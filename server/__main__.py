import sys
from optparse import OptionParser

from __start__ import __start__
from core.config import Config
from support.__constants__ import (DEFAULT_LHOST, DEFAULT_LPORT, PROGRAM,
                                   __date__, __version__)


def main():
    opts = OptionParser(
        usage=(f'./{PROGRAM} [OPTIONS]\n'
            f'v{__version__} ({__date__})')
    )

    opts.add_option('-p', '--port', type=int, default=DEFAULT_LPORT,
                    help=f'The port to listen on (default: {DEFAULT_LPORT})')

    opts.add_option('-a', '--address', type=str, default=DEFAULT_LHOST,
                    help=f'The address to listen on (default: {DEFAULT_LHOST})')

    (o, _) = opts.parse_args()
    
    conf = Config(
        lport=o.port,
        lhost=o.address,
    )

    return(__start__(conf))

if __name__ == "__main__":
    sys.exit(main())