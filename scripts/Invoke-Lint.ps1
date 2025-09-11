#!/usr/bin/env pwsh

# SPDX-FileCopyrightText: 2021 Shuai Zhang
#
# SPDX-License-Identifier: Apache-2.0

#Requires -Version 7.5

<#!
.SYNOPSIS
Run repository-wide static lint checks.

.DESCRIPTION
Sequentially runs the following tools for compliance and style validation:

1. reuse lint
	- Invoked via 'uvx reuse lint' to verify SPDX copyright & license headers.
2. markdownlint-cli2
	- Run through 'pnpm dlx' against all Markdown files ("**/*.md").
3. clang-format (dry-run)
	- Checks tracked C/C++ headers/sources ("*.h" "*.cc") using Google style with --dry-run --Werror; fails if formatting changes are needed.

Requirements:
- PowerShell 7.5+
- uv installed (https://github.com/astral-sh/uv) for 'uvx'
- pnpm installed (https://pnpm.io/) for markdownlint
- At least one tracked C/C++ file for clang-format to act on (otherwise that step is a no-op success)

Error Handling:
Any non-zero exit code from a step raises and causes overall exit code 1.

.EXAMPLE
PS> ./scripts/Invoke-Lint.ps1 -Verbose
Show verbose log output while executing all lint steps.

.NOTES
The script Push-Location's to the repository root, then restores the original location in 'finally'.
You can extend this script with additional static analysis steps (spell checking, YAML validation, etc.).
!#>

[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$InformationPreference = 'Continue'
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true
$PSStyle.OutputRendering = 'Host'  # Ensure ANSI rendering behavior is stable

try {
	Write-Verbose 'Switching to repository root (parent directory of script)'
	Push-Location (Join-Path $PSScriptRoot '..')

	Write-Verbose 'Executing uvx reuse lint ...'
    uvx reuse lint

    Write-Verbose 'Executing markdownlint ...'
    pnpm dlx markdownlint-cli2 "**/*.md"

    Write-Verbose 'Executing clang-format-diff ...'
    uvx clang-format --dry-run --Werror --style=Google `
        (git ls-files -- "*.h" "*.cc")
}
catch {
	Write-Error -ErrorRecord $_
	exit 1
}
finally {
	Pop-Location -ErrorAction SilentlyContinue | Out-Null
}
