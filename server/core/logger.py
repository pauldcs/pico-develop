import logging
from enum import Enum

from support.__constants__ import DEFAULT_LOGFILE

class LogLevel(Enum):
    INFO = "info"
    DEBUG = "debug"
    WARNING = "warning"
    ERROR = "error"
    FATAL = "fatal"

class Logger:
    LOG_LEVEL_MAP = {
        LogLevel.INFO: logging.INFO,
        LogLevel.DEBUG: logging.DEBUG,
        LogLevel.WARNING: logging.WARNING,
        LogLevel.ERROR: logging.ERROR,
        LogLevel.FATAL: logging.CRITICAL,
    }

    def __init__(self, name: str, level: LogLevel = LogLevel.INFO, to_console: bool = True, to_file: bool = True):
        self.logger = logging.getLogger(name)
        self.logger.setLevel(self._map_log_level(level))

        self.console_logger = logging.getLogger(name + '_console')
        self.console_logger.setLevel(self._map_log_level(level))

        self.file_logger = logging.getLogger(name)
        self.file_logger.setLevel(self._map_log_level(level))

        formatter = logging.Formatter(fmt="{levelname}: {asctime}: {message}", style="{")
        
        if to_console:
            console_handler = logging.StreamHandler()
            console_handler.setFormatter(formatter)
            self.console_logger.addHandler(console_handler)

        if to_file:
            file_handler = logging.FileHandler(DEFAULT_LOGFILE)
            file_handler.setFormatter(formatter)
            self.file_logger.addHandler(file_handler)

    @staticmethod
    def _map_log_level(level: LogLevel) -> int:
        return Logger.LOG_LEVEL_MAP.get(level, logging.INFO)

    def log(
            self,
            level: LogLevel,
            message: str,
            to_console: bool = True,
            to_file: bool = True
        ) -> None:
    
        log_level = Logger.LOG_LEVEL_MAP.get(level, logging.INFO)

        if to_console:
            self.console_logger.log(log_level, message)

        if to_file:
            self.file_logger.log(log_level, message)