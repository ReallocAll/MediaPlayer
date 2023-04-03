import os
from argparse import ArgumentParser
from typing import NamedTuple

import pynbs

parser = ArgumentParser()
parser.add_argument("filename", type=str, help="nbs filename")
parser.add_argument(
    "--out",
    "-o",
    type=str,
    default=None,
    help="output header file name",
)


class ProcessedNote(NamedTuple):
    sleep_time: int
    sound_index: int
    volume: float
    pitch: float


def process_nbs(nbs: pynbs.File) -> list[ProcessedNote]:
    header: pynbs.Header = nbs.header
    notes: list[pynbs.Note] = nbs.notes
    layers: list[pynbs.Layer] = nbs.layers

    time_per_tick = 20 / header.tempo * 50

    processed: list[ProcessedNote] = []
    for note in notes:
        layer = layers[note.layer]

        inst_pitch = 45
        sound_index = note.instrument if 0 <= note.instrument <= 15 else 0
        volume = (note.velocity / 100) * (layer.volume / 100)
        final_key = note.key + (inst_pitch - 45) + note.pitch / 100
        pitch = 2 ** ((final_key - 45) / 12)

        note_time = int(note.tick * time_per_tick)
        sleep_time = note_time

        processed.append(ProcessedNote(sleep_time, sound_index, volume, pitch))

    return processed


def gen_header(processed: list[ProcessedNote]) -> str:
    head = (
        f"#define MAX_NOTE_LEN {len(processed)}\n"
        "\n"
        "const float NOTE_DATA[MAX_NOTE_LEN][4] = {\n"
    )
    body = "".join(
        [f"\t{{{a}.0f, {b}.0f, {c}f, {round(d, 6)}f}},\n" for a, b, c, d in processed]
    )
    tail = "};\n"

    return f"{head}{body}{tail}"


def main():
    args = parser.parse_args()
    filename: str = args.filename
    out_path: str = args.out or f"{os.path.splitext(args.filename)[0]}.h"

    if not os.path.exists(filename):
        print("nbs file not exists")

    try:
        nbs = pynbs.read(filename)
    except BaseException:
        print("error when reading nbs file")
        raise

    processed = process_nbs(nbs)
    header = gen_header(processed)

    try:
        with open(out_path, "w", encoding="u8") as f:
            f.write(header)
    except BaseException:
        print("error when writing header file")
        raise

    print(f"file path: {os.path.abspath(out_path)}")
    print("success!")


if __name__ == "__main__":
    main()
