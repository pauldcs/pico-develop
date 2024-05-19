import select
import socket
import sys
import time
from abc import ABC, abstractmethod

from core.config import Config
from core.logger import Logger, LogLevel


def hexdump(data, bytes_per_line=32):
    result = ""
    
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        
        # Hexadecimal representation with four 8-byte groups separated by spaces
        hex_line = ' '.join(f'{b:02X}' for b in chunk[:8]) + ' ' + ' '.join(f'{b:02X}' for b in chunk[8:16]) + ' ' + ' '.join(f'{b:02X}' for b in chunk[16:24]) + ' ' + ' '.join(f'{b:02X}' for b in chunk[24:])
        
        # ASCII representation with non-printable characters replaced by a dot
        ascii_line = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        
        # Combine the formatted lines
        result += f'{hex_line.ljust(32 * 3)}  {ascii_line}\n'
    
    return result


class Command(ABC):
    @abstractmethod
    def execute(self):
        pass

class CloseSocketCommand(Command):
    def __init__(self, socket_obj, logger):
        self.socket_obj = socket_obj
        self.logger = logger

    def execute(self):
        try:
            if self.socket_obj:
                self.socket_obj.shutdown(socket.SHUT_RDWR)
        except OSError as e:
            self.logger.log(
                'error', f'Failed to shut down socket: {e}'
            )

        try:
            if self.socket_obj:
                self.socket_obj.close()
  
        except OSError as e:
            self.logger.log(
                'error', f'Failed while closing socket: {e}'
            )


class Server:
    def __init__(self, config: Config):
        self._logger   = Logger("server", LogLevel.DEBUG)
        self._lhost    = config.lhost
        self._lport    = config.lport
        self._srv_sock = None
        self._cli_sock = None
        self._cli_addr = None

    def bind_server_socket(self):
        retry_delay = 1
    
        while True:
            try:
                self._srv_sock.bind((
                    self._lhost,
                    self._lport
                ))

                self._srv_sock.listen(1)
                self._logger.log(
                    'info', f'Listening on {self._lhost}:{self._lport}'
                )
                break

            except OSError as e:
                if e.errno == 98:
                    self._logger.log(
                        'warning',
                        f'Address already in use.'
                        f'Retrying in {retry_delay} seconds...'
                    )
                    time.sleep(retry_delay)
  
                else:
                    raise

    def accept_new_connection(self):
        self._cli_sock, self._cli_addr = self._srv_sock.accept()
        self._logger.log(
            'info', f'Accepted connection from {self._cli_addr}'
        )

    def handle_data_from_client(self):
        _func_name = 'handle_data_from_client'
  
        try:
            data = self._cli_sock.recv(4096)
            if not data:
                self._logger.log('info', 'Client disconnected')
                self._cli_sock.close()
                self._cli_sock = None
                return False
    
        except socket.error as e:
            self._logger.log(
                'error', f'{_func_name}: socket error: {e}',
            )

            return False
  
        except Exception as e:
            self._logger.log(
                'error', f'{_func_name}: error receiving data: {e}',
            )

            return False

        self._logger.log(
            'info',
            f'<- {len(data)}'
            f' bytes received '
            f'{self._cli_sock}',
            to_console=False
        )
  
        self._logger.log(
            'info', "dumping data...\n" + hexdump(data),
            to_console=False
        )

        #sys.stdout.buffer.write(data)
        #sys.stdout.flush()

        return True

    def start_server(self):
        close_socket_command = CloseSocketCommand(None, self._logger)
        delay = 0.1

        self._srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._srv_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        self.bind_server_socket()

        while True:
            if not self._cli_sock:
                self.accept_new_connection()

            rlist, _, _ = select.select([self._cli_sock, sys.stdin], [], [])

            for src in rlist:
                if src is self._cli_sock:
                    if not self.handle_data_from_client():
                        close_socket_command.execute()
                        self._cli_sock = None
                        break

            if delay != 0:
                time.sleep(delay)