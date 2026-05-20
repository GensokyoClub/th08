#!/usr/bin/env python3

import argparse
import os
import ghidra_helpers

from pathlib import Path

SCRIPT_PATH = Path(os.path.realpath(__file__)).parent


def main():
    parser = argparse.ArgumentParser(
        description="Export Ghidra symbols to CSV files compatible with reccmp",
    )
    parser.add_argument("GHIDRA_REPO_NAME")
    parser.add_argument(
        "--username", help="Username to use when connecting to the ghidra server."
    )
    parser.add_argument(
        "--ssh-key",
        help="""SSH key to use to authenticate to a ghidra server.
                        Note that the ghidra server must have SSH authentication enabled for this to work.
                        To enable SSH auth, add -ssh in the wrapper.parameters of the Ghidra Server's server.conf""",
    )
    parser.add_argument("--program", help="Program to export")
    parser.add_argument("--project-name")
    args = parser.parse_args()

    ghidra_helpers.runAnalyze(
        args.GHIDRA_REPO_NAME,
        project_name=args.project_name,
        process=args.program,
        username=args.username,
        ssh_key=args.ssh_key,
        pre_scripts=[["ExportGhidraToReccmp.java"]],
    )


if __name__ == "__main__":
    main()
