from     typing import Optional
from contextlib import contextmanager
import os

class File:
    def __init__(self, file_path: str):
        self._file_path = file_path

    @property
    def exists(self) -> bool:
        return os.path.exists(self._file_path)

    @property
    def is_file(self) -> bool:
        return os.path.isfile(self._file_path)

    @property
    def is_dir(self) -> bool:
        return os.path.isdir(self._file_path)

    @property
    def size(self) -> Optional[int]:
        return os.path.getsize(self._file_path)
  
    @property
    def name(self) -> Optional[int]:
        if (os.path.basename(self._file_path) != ''):
            return os.path.basename(self._file_path)
        return self._file_path

    @contextmanager
    def open(self, mode: str = 'r'):
        if self.exists:
            with open(self._file_path, mode) as file:
                yield file
        else:
            yield None

    def read(self) -> Optional[str]:
        with self.open('r') as file:
            if file:
                return file.read()
        return None

    def write(self, content: str) -> None:
        with self.open('w') as file:
            if file:
                file.write(content)

    def append(self, content: str) -> None:
        with self.open('a') as file:
            if file:
                file.write(content)