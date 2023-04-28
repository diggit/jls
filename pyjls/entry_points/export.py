# Copyright 2023 Jetperch LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from pyjls import Reader, SignalType
import logging
import numpy as np


log = logging.getLogger(__name__)
_BLOCK_SIZE = 1024 * 1024


def parser_config(p):
    """Export JLS data."""
    p.add_argument('infile',
                   help='JLS input filename')
    p.add_argument('--signal_id',
                   type=int,
                   help='The signal id to export.')
    p.add_argument('--start',
                   type=int,
                   help='The starting timestamp (inclusive).')
    p.add_argument('--length',
                   type=int,
                   help='The number of samples to export.')
    p.add_argument('outfile',
                   help='The output filename')
    return on_cmd


def on_cmd(args):
    with Reader(args.infile) as r:
        if args.signal_id is None:
            signal_id = list(r.signals.keys())[1]
        else:
            signal_id = int(args.signal_id)
        signal = r.signals[signal_id]
        if signal.signal_type != SignalType.FSR:
            print('Signal is not FSR')
            return 1
        start = 0 if args.start is None else int(args.start)
        if start >= signal.length:
            print(f'start {start} is past length {signal.length}')
            return 1
        length = signal.length if args.length is None else int(args.length)
        if (start + length) > signal.length:
            new_length = signal.length - start
            print(f'reducing length from {length} to {new_length}')
            length = new_length

        nan_count = 0
        with open(args.outfile, 'wb') as f:
            while length > 0:
                k = _BLOCK_SIZE if length > _BLOCK_SIZE else length
                data = r.fsr(signal_id, start, k)
                nan_count += np.count_nonzero(np.isnan(data))
                data.tofile(f)
                start += k
                length -= k
        print(f'nan count = {nan_count}')
