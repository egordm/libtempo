import os
import struct
import numpy as np

TYPE_DOUBLE = 0x1
TYPE_COMPLEX = 0x16


class DataFile:
    __slots__ = 'data', 'extra', 'parameters'

    def __init__(self, data, extra) -> None:
        self.data = data
        self.extra = extra


def read_data_file(path):
    with open(path, 'rb') as f:
        rows = struct.unpack('Q', f.read(8))[0]
        cols = struct.unpack('Q', f.read(8))[0]
        type_mask = f.read(1)[0]
        n_extra = struct.unpack('i', f.read(4))[0]
        extra = f.read(n_extra)

        is_complex = (type_mask & TYPE_COMPLEX) != 0
        dtype = np.int
        if (type_mask & TYPE_DOUBLE) != 0:
            if is_complex:
                dtype = np.complex128
            else:
                dtype = np.float64

        tmp = f.read(np.dtype(dtype).itemsize * rows * cols)
        data = np.frombuffer(tmp, dtype=dtype)
        if cols > 1 and rows > 1: data = np.transpose(data.reshape((cols, rows)))

    return DataFile(data, extra)


class ObjectDict(dict):
    """
    Object like dict, every dict[key] can be visited by dict.key
    """

    def __getattr__(self, name):
        return self.__getitem__(name)


def load_folder(path):
    ret = ObjectDict()

    for file in os.listdir(path):
        file_path = os.path.join(path, file)
        if not os.path.isfile(file_path) or not file.endswith('.npd'): continue
        filename, file_extension = os.path.splitext(file)

        ret[filename] = parse_known_extras(filename, read_data_file(file_path))

    return ret


def parse_known_extras(name: str, data: DataFile):
    if name == 'novelty_curve':
        data.parameters = dict(feature_rate=struct.unpack('i', data.extra)[0])
    if name == 'tempogram_cyclic':
        data.parameters = dict(ref_tempo=struct.unpack('i', data.extra)[0])
    if name == 'smooth_tempogram':
        data.parameters = dict(smooth_length=struct.unpack('i', data.extra)[0])
    if name == 'tempo_curve':
        data.parameters = dict(min_section_length=struct.unpack('i', data.extra)[0])

    return data


class Section:
    __slots__ = 'start', 'end', 'bpm', 'offset'

    def __init__(self, start, end, bpm, offset) -> None:
        self.start = start
        self.end = end
        self.bpm = bpm
        self.offset = offset


def load_sections(path):
    if not os.path.exists(path): raise Exception(f'No such file {path}')
    with open(path, 'r') as f:
        lines = f.readlines()

    ret = []
    for line in lines:
        ret.append(Section(*[float(token) for token in line.split(',')]))

    return ret
