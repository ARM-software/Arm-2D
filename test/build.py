#!/usr/bin/python
# -*- coding: utf-8 -*-

import logging

from datetime import datetime
from enum import Enum
from glob import glob, iglob
from pathlib import Path
from shutil import copy

from zipfile import ZipFile

from matrix_runner import main, matrix_axis, matrix_action, matrix_command


@matrix_axis("project", "p", "Project(s) to be considered.")
class ProjectAxis(Enum):
    GENERIC = ('generic', 'gen')
    SMARTWATCH = ('smartwatch', 'sw')


@matrix_axis("device", "d", "Device(s) to be considered.")
class DeviceAxis(Enum):
    CM7    = ('Cortex-M7',  'CM7', 'VHT_M7')
    CM33   = ('Cortex-M33', 'CM33', 'VHT_M33')
    CM55   = ('Cortex-M55', 'CM55', 'VHT_M55')
    SSE300 = ('Corstone_SSE-300', 'SSE300', 'VHT-Corstone-300')
    SSE310 = ('Corstone_SSE-310', 'SSE310', 'VHT-Corstone-310')


@matrix_axis("compiler", "c", "Compiler(s) to be considered.")
class CompilerAxis(Enum):
    AC6 = ('AC6')
    GCC = ('GCC')
    IAR = ('IAR')

    @property
    def image_ext(self):
        ext = {
            CompilerAxis.AC6: 'axf',
            CompilerAxis.GCC: 'elf',
            CompilerAxis.IAR: 'elf'
        }
        return ext[self]


@matrix_axis("optimize", "o", "Optimization to be considered.")
class OptimizeAxis(Enum):
    DEBUG = ('Debug')
    RELEASE  = ('Release')


MODEL_EXECUTABLE = {
    DeviceAxis.CM7: ("VHT_MPS2_Cortex-M7", []),
    DeviceAxis.CM33: ("VHT_MPS2_Cortex-M33", []),
    DeviceAxis.CM55: ("VHT_MPS2_Cortex-M55", []),
    DeviceAxis.SSE300: ("VHT_MPS3_Corstone_SSE-300", []),
    DeviceAxis.SSE310: ("VHT_Corstone_SSE-310", [])
}


def config_suffix(config, timestamp=True):
    suffix = f"{config.compiler[0]}-{config.optimize}-{config.device[1]}"
    if timestamp:
        suffix += f"-{datetime.now().strftime('%Y%m%d%H%M%S')}"
    return suffix


def project_name(config):
    return f"arm2d_{config.project}.{config.optimize}+{config.device[2]}"


def project_dir(config):
    return f"{project_name(config)}-{config.compiler}"


def project_outdir(config):
    return f"{project_dir(config)}/outdir"


def model_config(config):
    return f"model_config_{config.device[1].lower()}.txt"


@matrix_action
def clean(config):
    """Build the selected configurations using CMSIS-Build."""
    yield cbuild_clean(f"{project_dir(config)}/{project_name(config)}.cprj")


@matrix_action
def build(config, results):
    """Build the selected configurations using CMSIS-Build."""
    logging.info("Compiling Project...")

    src = Path(f"arm2d.{config.compiler[0].lower()}-cdefault.yml")
    dst = Path("arm2d.cdefault.yml")
    dst.unlink(missing_ok=True)
    copy(src, dst)

    yield csolution(f"{project_name(config)}")
    yield cbuild(f"{project_dir(config)}/{project_name(config)}.cprj")

    if not all(r.success for r in results):
        return

    file = f"blinky-{config_suffix(config)}.zip"
    logging.info(f"Archiving build output to {file}...")
    with ZipFile(file, "w") as archive:
        for content in iglob(f"{project_dir(config)}/**/*", recursive=True):
            if Path(content).is_file():
                archive.write(content)


@matrix_action
def extract(config):
    """Extract the latest build archive."""
    archives = sorted(glob(f"EventStatistic-{config_suffix(config, timestamp=False)}-*.zip"), reverse=True)
    yield unzip(archives[0])


@matrix_action
def run(config, results):
    """Run the selected configurations."""
    logging.info("Running Event Statistic Example on Arm model ...")
    yield model_exec(config)


@matrix_command()
def cbuild_clean(project):
    return ["cbuild", "-c", project]


@matrix_command()
def unzip(archive):
    return ["bash", "-c", f"unzip {archive}"]


@matrix_command()
def csolution(project):
    return ["csolution", "convert", "-s", "arm2d.csolution.yml", "-c", project]


@matrix_command()
def cbuild(project):
    return ["cbuild", project]


@matrix_command()
def model_exec(config):
    cmdline = [MODEL_EXECUTABLE[config.device][0], "-q", "--simlimit", 200, "-f", model_config(config)]
    cmdline += MODEL_EXECUTABLE[config.device][1]
    cmdline += ["-a", f"{project_outdir(config)}/{project_name(config)}.{config.compiler.image_ext}"]
    return cmdline


if __name__ == "__main__":
    main()
