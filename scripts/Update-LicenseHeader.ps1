#!/usr/bin/env pwsh

# SPDX-FileCopyrightText: 2021 Shuai Zhang
#
# SPDX-License-Identifier: Apache-2.0

#Requires -Version 7.5
<#!
.SYNOPSIS
Add (or update) license headers for repository documentation files in bulk.

.DESCRIPTION
Invokes 'reuse annotate' via 'uvx' to write copyright and license headers
into project files. Documentation (Markdown) files get a documentation
license, while source / configuration / template files receive the default
project license. Patterns are hard-coded in the script (see uvx reuse
annotate commands below).

Requires 'uv' installed (https://github.com/astral-sh/uv). This script uses
'uvx reuse annotate' to fetch/run the reuse tool on demand.

.
.PARAMETER Copyright
Copyright holder string passed to 'reuse annotate' (-c).

.PARAMETER DefaultLicense
SPDX license identifier applied to general (non-documentation) files
such as source code, configuration, overrides, and scripts.

.PARAMETER DocumentLicense
SPDX license identifier applied specifically to documentation-oriented
files (e.g. Markdown in docs/).

.EXAMPLE
PS> ./Update-LicenseHeader.ps1 -Verbose
Runs the annotation process using default copyright and licenses.

.NOTES
Run 'uvx reuse lint' beforehand if you wish to validate existing headers.
!#>

[CmdletBinding()]
param(
	[Parameter()]
	[ValidateNotNullOrEmpty()]
	[string]$Copyright = 'Shuai Zhang',

	[Parameter()]
	[ValidatePattern('^[A-Za-z0-9.-]+$')]
	[string]$DefaultLicense = 'Apache-2.0',

	[Parameter()]
	[ValidatePattern('^[A-Za-z0-9.-]+$')]
	[string]$DocumentLicense = 'CC-BY-NC-ND-4.0'
)

Set-StrictMode -Version Latest
$InformationPreference = 'Continue'
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true
$PSStyle.OutputRendering = 'Host'  # Ensure ANSI rendering behavior is stable

try {
	Write-Verbose 'Switching to repository root (parent directory of script)'
	Push-Location (Join-Path $PSScriptRoot '..')

	Write-Verbose 'Executing uvx reuse annotate ...'

	uvx reuse annotate -c $Copyright -l $DocumentLicense -y 2021 `
        "docs/**/*.md"
    # Add "docs/**/*.js" here if you want license headers in JS files too
	uvx reuse annotate -c $Copyright -l $DefaultLicense -y 2021 `
        "docs/**/*.html" "docs/**/*.css"
	# uvx reuse annotate -c $Copyright -l $DefaultLicense -y 2021 `
    #     "docs/**/*.h" "docs/**/*.c" "docs/**/*.cc" "docs/**/*.java"

	uvx reuse annotate -c $Copyright -l $DefaultLicense -y 2021 `
        "overrides/**" "scripts/**"

	uvx reuse annotate -c $Copyright -l $DefaultLicense -y 2021 `
        ".editorconfig" ".gitignore" ".markdownlint-cli2.yaml" ".markdownlint.yml" `
        "mkdocs.yml" "pyproject.toml" "README.*" "REUSE.toml"

	Write-Information 'Completed reuse annotate.'
}
catch {
	Write-Error -ErrorRecord $_
	exit 1
}
finally {
	Pop-Location -ErrorAction SilentlyContinue | Out-Null
}
