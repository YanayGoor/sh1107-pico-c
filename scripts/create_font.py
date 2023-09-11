import glob
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable
from textwrap import indent

from bitarray import bitarray
from bitarray.util import ba2hex
import click
from PIL import ImageFont
from string import printable

from click import BadParameter

BITS_PER_HEX_CHAR = 4
FONT_SIZE_STEP = 8


@dataclass
class FontChar:
    buff: bitarray
    size: tuple[int, int]
    offset: tuple[int, int]


FontSize = dict[str, FontChar]
Font = dict[int, FontSize]


FONT_HEADERS_DIR = Path(__file__).parent.parent / "include" / "fonts"
FONT_SRCS_DIR = Path(__file__).parent.parent / "src" / "fonts"


def render_char(char: str, *, font: str, size: int) -> FontChar:
    ttf = ImageFont.truetype(font, size=size)
    image, offset = ttf.getmask2(char, mode="1", fill=1)

    return FontChar(
        buff=bitarray(
            [
                image.getpixel((i, j)) != 0
                for j in range(image.size[1])
                for i in range(image.size[0])
            ],
            endian="little",  # so the buffer index easily corresponds to the coords
        ),
        size=image.size,
        offset=offset,
    )


def render_font_size(font: str, size: int) -> FontSize:
    return {char: render_char(char, font=font, size=size) for char in printable}


def render_font(font: str, *, sizes: Iterable[int]) -> Font:
    return {size: render_font_size(font=font, size=size) for size in sizes}


def char_to_header(char: str, rendered: FontChar) -> str:
    padding = bitarray((BITS_PER_HEX_CHAR - len(rendered.buff)) % BITS_PER_HEX_CHAR)
    return indent(
        f"""
[{ord(char)}] = {{
    .width = {rendered.size[0]},
    .height = {rendered.size[1]},
    .buff = "{ba2hex(rendered.buff + padding)}",
    .offset = {{ {rendered.offset[0]}, {rendered.offset[1]} }}
}}""",
        "\t",
    )


def font_size_to_header(size: int, rendered: FontSize) -> str:
    chars = [char_to_header(char, rendered[char]) for char in rendered.keys()]
    return indent(
        f"""
[{size // FONT_SIZE_STEP}] = {{{", ".join(chars)}
}}""",
        "\t",
    )


def font_to_source_file(font_name: str, rendered: Font) -> str:
    font = ",".join(font_size_to_header(size, rendered[size]) for size in rendered)
    return f"""
#include "font.h"

font_t font_{font_name} = {{{font}
}};
"""

def font_to_header_file(font_name: str) -> str:
    return f"""
#ifndef FONT_{font_name.upper()}_H
#define FONT_{font_name.upper()}_H
#include "../font.h"

extern font_t font_{font_name};
#endif // FONT_{font_name.upper()}_H        
"""


def validate_size(_, __, sizes):
    if len(sizes) == 0:
        raise BadParameter("no size given")
    if any(size % FONT_SIZE_STEP != 0 for size in sizes):
        raise BadParameter(f"size must be a multiple of {FONT_SIZE_STEP}")
    return sizes


def validate_font(font: str):
    if Path(font).is_dir():
        return [
            Path(font) / p for p in glob.glob("**/*.ttf", recursive=True, root_dir=font)
        ]
    if Path(font).suffix == ".ttf":
        return [Path(font)]
    raise BadParameter("font must be directory or .ttf file")


def validate_fonts(_, __, fonts: tuple[str]):
    return [p for f in fonts for p in validate_font(f)]


@click.command()
@click.option(
    "-s",
    "--size",
    multiple=True,
    type=int,
    callback=validate_size,
    default=(16, 24, 32),
)
@click.option(
    "-f",
    "--font",
    multiple=True,
    type=click.Path(exists=True),
    callback=validate_fonts,
    default=(FONT_HEADERS_DIR,),
)
def main(size: tuple[int], font: list[Path]):
    for file in font:
        font_name = Path(file).stem
        font = render_font(str(file), sizes=size)
        with open(FONT_SRCS_DIR / f"{font_name}.c", mode="w+") as f:
            f.write(font_to_source_file(font_name, font))
        with open(FONT_HEADERS_DIR / f"{font_name}.h", mode="w+") as f:
            f.write(font_to_header_file(font_name))


if __name__ == "__main__":
    main()
