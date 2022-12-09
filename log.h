#pragma once
#ifndef __CCH_LOG_H__
#define __CCH_LOG_H__

#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>
#include<map>
#include"singleton.h"
#include"util.h" 


#define CCH_LOG_LEVEL(logger, level) \
	if(logger->getLevel() <= level) \
		cch::LogEventWrap(cch::LogEvent::ptr(new cch::LogEvent(logger, level, __FILE__, __LINE__, 0, cch::GetThreadId(), \ 
			cch::GetFiberId(), time(0)))).getSS()

#define CCH_LOG_DEBUG(logger) CCH_LOG_LEVEL(logger, cch::LogLevel::DEBUG)
#define CCH_LOG_ERROR(logger) CCH_LOG_LEVEL(logger, cch::LogLevel::ERROR)
#define CCH_LOG_FATAL(logger) CCH_LOG_LEVEL(logger, cch::LogLevel::FATAL)
#define CCH_LOG_INFO(logger) CCH_LOG_LEVEL(logger, cch::LogLevel::INFO)
#define CCH_LOG_WARN(logger) CCH_LOG_LEVEL(logger, cch::LogLevel::WARN)

#define CCH_LOG_FMT_LEVEL(logger, level, fmt, ...) \
	if(logger->getLevel()<=level) \
		cch::LogEventWrap(cch::LogEvent::ptr(new cch::LogEvent(logger, level, __FILE__, __LINE__, \
			0, cch::GetThreadId(), cch::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define CCH_LOG_FMT_DEBUG(logger, fmt, ...) CCH_LOG_FMT_LEVEL(logger, cch::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define CCH_LOG_FMT_ERROR(logger, fmt, ...) CCH_LOG_FMT_LEVEL(logger, cch::LogLevel::ERROR, fmt, __VA_ARGS__)
#define CCH_LOG_FMT_FATAL(logger, fmt, ...) CCH_LOG_FMT_LEVEL(logger, cch::LogLevel::FATAL, fmt, __VA_ARGS__)
#define CCH_LOG_FMT_INFO(logger, fmt, ...) CCH_LOG_FMT_LEVEL(logger, cch::LogLevel::INFO, fmt, __VA_ARGS__)
#define CCH_LOG_FMT_WARN(logger, fmt, ...) CCH_LOG_FMT_LEVEL(logger, cch::LogLevel::WARN, fmt, __VA_ARGS__)

#define CCH_LOG_ROOT() cch::loggerMgr::GetInstance()->getRoot()
#define CCH_LOG_NAME(name) cch::loggerMgr::GetInstance()->getLogger(name)

//日志模块
namespace cch {
	class Logger;
	class LoggerManager;

	//日志级别
	class LogLevel
	{
	public:
		enum Level {
			UNKNOW = 0,
			DEBUG = 1,
			INFO = 2,
			WARN = 3,
			ERROR = 4,
			FATAL = 5
		};

		static const char* ToString(LogLevel::Level level);
		static LogLevel::Level FromString(const std::string& str);
	};

	//日志事件
	class LogEvent
	{
	public:
		typedef std::shared_ptr<LogEvent> ptr;
		LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse,
			uint32_t threadid, uint32_t fiberid, uint64_t time);
		~LogEvent();
		const char* getFile() const { return m_file; }
		int32_t getLine() const { return m_line; }
		uint32_t gettElapse() const { return m_elapse; }
		uint32_t getThreadId() const { return m_threadid; }
		uint32_t getFiberId() const { return m_fiberid; }
		uint64_t getTime() const { return m_time; }
		std::string getContent() { return m_ss.str(); }
		std::shared_ptr<Logger> getLogger() const { return m_logger; }
		LogLevel::Level getLevel() const { return m_level; }
		std::stringstream& getSS() { return m_ss; }
		void format(const char* fmt, ...);
		void format(const char* fmt, va_list al);
	private:
		const char* m_file = nullptr;  //文件名
		int32_t m_line = 0; //行号
		uint32_t m_elapse = 0;	//程序启动开始到现在的毫秒数
		uint32_t m_threadid = 0; //线程id
		uint32_t m_fiberid = 0; //协程id
		uint64_t m_time = 0;	//时间戳
		std::stringstream m_ss; //内容

		std::shared_ptr<Logger> m_logger;
		LogLevel::Level m_level;
	};

	class LogEventWrap
	{
	public:
		LogEventWrap(LogEvent::ptr e);
		~LogEventWrap();
		std::stringstream& getSS();
		LogEvent::ptr getEvent() const { return m_event; }
	private:
		LogEvent::ptr m_event;
	};


	//日志格式器
	class LogFormatter
	{
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
		LogFormatter(const std::string& pattern);
	public:
		class FormatItem
		{
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			//FormatItem(const std::string fmt = "") {};
			virtual ~FormatItem(){}
			virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		};

		void init();

		bool isError() const { return m_error; }
		const std::string getPattern() const { return m_pattern; }
	private:
		std::vector<FormatItem::ptr> m_items;
		std::string m_pattern;
		bool m_error = false;
	};

	//日志输出地
	class LogAppender
	{
	public:
		typedef std::shared_ptr<LogAppender> ptr;
		virtual ~LogAppender() {}
		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		virtual std::string toYamlString() = 0;
		void setFormatter(LogFormatter::ptr val);
		LogFormatter::ptr getFormatter() const;
		void setLevel(LogLevel::Level level) { m_level = level; };
		LogLevel::Level getLevel() const { return m_level; }
	protected:
		LogLevel::Level m_level = LogLevel::DEBUG;
		LogFormatter::ptr m_formatter;
	};


	//日志器
	class Logger: public std::enable_shared_from_this<Logger>
	{
		friend class LoggerManager;
	public:
		typedef std::shared_ptr<Logger> ptr;

		Logger(const std::string& name = "root");

		void log(LogLevel::Level level, LogEvent::ptr event);

		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);

		void addAppender(LogAppender::ptr appender);
		void delAppender(LogAppender::ptr appender);
		void clearAppenders();
		LogLevel::Level getLevel() const;
		void setLevel(LogLevel::Level level);
		const std::string& getName() const { return m_name; }

		void setFormatter(LogFormatter::ptr val);
		void setFormatter(const std::string& val);
		LogFormatter::ptr getFormatter() const { return m_formatter; };

		std::string toYamlString();
	private:
		std::string m_name;	//日志名称
		LogLevel::Level m_level;
		std::list<LogAppender::ptr> m_appenders; //appender集合
		LogFormatter::ptr m_formatter;
		Logger::ptr m_root;
		//LogEvent::ptr	
	};

	//输出到控制台的appender
	class StdoutLogAppender : public LogAppender
	{
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;
		void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
		std::string toYamlString() override;
	private:
	};

	//输出到文件的appender
	class FileLogAppender : public LogAppender
	{
	public:
		typedef std::shared_ptr<FileLogAppender> ptr;
		void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
		FileLogAppender(const std::string &filename);
		bool reopen();//重新打开文件，文件打开成功，返回true
		std::string toYamlString() override;
	private:
		std::string m_filename;
		std::ofstream m_filestream;
	};

	class LoggerManager
	{
	public:
		LoggerManager();
		Logger::ptr getLogger(const std::string& name);
		void init();
		Logger::ptr getRoot() const { return m_root; };
		std::string toYamlString();
	private:
		std::map<std::string, Logger::ptr> m_logger;
		Logger::ptr m_root;
	};

	typedef cch::Singleton<LoggerManager> loggerMgr;
}

#endif // !__CCH_LOG_H__


