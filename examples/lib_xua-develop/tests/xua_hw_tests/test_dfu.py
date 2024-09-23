# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from pathlib import Path
import platform
import pytest
import re
import subprocess

from hardware_test_tools.UaDut import UaDut
from hardware_test_tools.UaDfuApp import UaDfuApp


@pytest.fixture(scope="module")
def factory_xe():
    xe_path = Path(__file__).parent / "test_dfu" / "bin" / "factory" / "dfu_test_factory.xe"
    assert xe_path.exists()
    return xe_path


def create_dfu_bin(xe_path):
    version_re = r"XTC version: (\d+)\.(\d+)\.(\d+)"
    ret = subprocess.run(["xcc", "--version"], capture_output=True, text=True, timeout=30)

    match = re.search(version_re, ret.stdout)
    assert match, f"Unable to get XTC Tools version: stdout={ret.stdout}"

    bin_path = xe_path.parent / f"{xe_path.stem}.bin"
    cmd = ["xflash", "--factory-version", f'{match.group(1)}.{match.group(2)}', "--upgrade", "1", xe_path, "-o", bin_path]
    ret = subprocess.run(cmd, text=True, capture_output=True, timeout=30)
    assert ret.returncode == 0, f"Failed to create DFU binary, cmd {cmd}\nstdout:\n{ret.stdout}\nstderr:\n{ret.stderr}"

    return bin_path


@pytest.fixture(scope="module")
def upgrade_bin():
    xe_path = Path(__file__).parent / "test_dfu" / "bin" / "upgrade" / "dfu_test_upgrade.xe"
    assert xe_path.exists()

    bin_path = create_dfu_bin(xe_path)
    assert bin_path.exists()
    return bin_path


def cfg_list():
    bin_dir = Path(__file__).parent / "test_dfu" / "bin"
    if not bin_dir.exists():
        return []
    exclude_cfgs = ["factory", "upgrade"]
    all_cfgs = [dir.stem for dir in bin_dir.iterdir()]
    return [cfg for cfg in all_cfgs if cfg not in exclude_cfgs]


def dfu_app_list():
    if platform.system() == "Windows":
        return ["custom"]
    elif platform.system() == "Darwin":
        return ["custom", "dfu-util"]
    else:
        # Until test subdirectories can be rearranged with XCommon CMake builds, an
        # empty list needs to be returned here for cases where a Linux agent collects
        # these tests from the top-level test directory (but would filter them out)
        return []


@pytest.mark.parametrize("cfg", cfg_list())
@pytest.mark.parametrize("dfu_app", dfu_app_list())
def test_dfu(pytestconfig, factory_xe, upgrade_bin, cfg, dfu_app):
    if cfg == "i2s_only" and platform.system() == "Windows":
        pytest.skip("i2s_only not supported on Windows")

    xtag_id = pytestconfig.getoption("xtag_id")
    assert xtag_id, "--xtag-id option must be provided on the command line"

    test_xe = Path(__file__).parent / "test_dfu" / "bin" / cfg / f"dfu_test_{cfg}.xe"
    test_bin = create_dfu_bin(test_xe)

    pid = 0x16
    in_chans = 2
    out_chans = 2
    prod_str = "XUA DFU Test"

    # factory -> cfg -> upgrade
    with UaDut(xtag_id, factory_xe, pid, prod_str, in_chans, out_chans, xflash=True) as dut:
        dfu_test = UaDfuApp(dut.driver_guid, pid, dfu_app_type=dfu_app)
        factory_version = dfu_test.get_bcd_version()

        dfu_test.download(test_bin)
        cfg_version = dfu_test.get_bcd_version()
        assert cfg_version != factory_version

        upload_file = Path(__file__).parent / "test_dfu_upload.bin"
        dfu_test.upload(upload_file)
        cfg_version2 = dfu_test.get_bcd_version()
        assert cfg_version2 == cfg_version

        dfu_test.download(upgrade_bin)
        upgrade_version = dfu_test.get_bcd_version()
        assert upgrade_version not in [factory_version, cfg_version]

        dfu_test.download(upload_file)
        upload_file.unlink()
        upload_version = dfu_test.get_bcd_version()
        assert upload_version == cfg_version

        dfu_test.revert_factory()
        revert_version = dfu_test.get_bcd_version()
        assert revert_version == factory_version
