# SPDX-FileCopyrightText: 2021 Shuai Zhang
#
# SPDX-License-Identifier: Apache-2.0

#Requires -Version 7.5

<#!
.SYNOPSIS
Submit URLs from a sitemap.xml file by using the IndexNow protocol.

.DESCRIPTION
Parses the specified sitemap, extracts the values of each <loc> element, and builds the JSON payload defined by the Bing IndexNow documentation (https://learn.microsoft.com/en-us/bingwebmaster/indexnow). The script enforces that all URLs share the same host, validates the site key, and batches requests when more than the protocol limit of 10,000 URLs must be submitted.

.PARAMETER SitemapPath
Path to the sitemap XML file. The default points to the generated site/sitemap.xml artifact in this repository.

.PARAMETER Endpoint
IndexNow endpoint URL. Defaults to https://www.bing.com/indexnow.

.PARAMETER UrlsPerRequest
Maximum number of URLs per IndexNow request. The protocol allows up to 10,000 and that value is used by default.

.PARAMETER Key
IndexNow site key. The key must be 8-128 characters and contain only letters, numbers, or dashes.

.PARAMETER KeyLocation
Publicly accessible URL of the key TXT file. When omitted, the script assumes https://<host>/<key>.txt (IndexNow option 1).

.PARAMETER DryRun
Skip the HTTP POST call and emit the payload that would be submitted.

.EXAMPLE
PS> ./scripts/Invoke-IndexNow.ps1 -Verbose
Submit URLs from site/sitemap.xml to Bing IndexNow while printing verbose diagnostics.

.EXAMPLE
PS> ./scripts/Invoke-IndexNow.ps1 -DryRun
Validate the request payload without contacting the IndexNow endpoint.

.NOTES
- Requires PowerShell 7.5 or later.
- Ensure the key TXT file is publicly reachable before executing the script.
- IndexNow allows mixing http/https URLs as long as they share the same host name.
!#>

[CmdletBinding(SupportsShouldProcess)]
param(
    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string]$SitemapPath = (Join-Path (Split-Path -Parent $PSScriptRoot) 'site/sitemap.xml'),

    [Parameter()]
    [ValidateNotNull()]
    [Uri]$Endpoint = [Uri]'https://www.bing.com/indexnow',

    [Parameter()]
    [ValidateRange(1, 10000)]
    [int]$UrlsPerRequest = 10000,

    [Parameter()]
    [ValidatePattern('^[A-Za-z0-9-]{8,128}$')]
    [string]$Key = '5f62d74a17a5413a82bafcc13ae96791',

    [Parameter()]
    [Uri]$KeyLocation,

    [switch]$DryRun
)

Set-StrictMode -Version Latest
$InformationPreference = 'Continue'
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

if (-not (Test-Path -LiteralPath $SitemapPath)) {
    throw "Sitemap file not found: $SitemapPath"
}

$resolvedSitemapPath = Resolve-Path -LiteralPath $SitemapPath
Write-Verbose "Resolved sitemap path: $resolvedSitemapPath"

[xml]$sitemapXml = Get-Content -LiteralPath $resolvedSitemapPath -Raw
$namespaceManager = [System.Xml.XmlNamespaceManager]::new($sitemapXml.NameTable)
$namespaceManager.AddNamespace('sm', 'http://www.sitemaps.org/schemas/sitemap/0.9') | Out-Null

$locNodes = $sitemapXml.SelectNodes('//sm:url/sm:loc', $namespaceManager)
if (-not $locNodes -or $locNodes.Count -eq 0) {
    throw 'No <loc> elements were found in the sitemap.'
}

$uriList = [System.Collections.Generic.List[System.Uri]]::new()
foreach ($node in $locNodes) {
    $url = $node.InnerText.Trim()
    if ([string]::IsNullOrWhiteSpace($url)) {
        continue
    }

    try {
        $uriList.Add([Uri]$url) | Out-Null
    }
    catch {
        throw "Invalid URL '$url' in sitemap: $($_.Exception.Message)"
    }
}

if ($uriList.Count -eq 0) {
    throw 'The sitemap did not contain any valid URLs.'
}

$distinctHosts = @($uriList | ForEach-Object { $_.Host.ToLowerInvariant() } | Sort-Object -Unique)
if ($distinctHosts.Length -ne 1) {
    throw "The sitemap contains multiple hosts: $($distinctHosts -join ', ')"
}

$hostName = $distinctHosts[0]
Write-Verbose "Resolved IndexNow host: $hostName"

if (-not $PSBoundParameters.ContainsKey('KeyLocation')) {
    $defaultScheme = $uriList[0].Scheme
    if ([string]::IsNullOrWhiteSpace($defaultScheme)) {
        $defaultScheme = 'https'
    }
    $builder = [System.UriBuilder]::new()
    $builder.Scheme = $defaultScheme
    $builder.Host = $hostName
    $builder.Path = "$Key.txt"
    $builder.Port = -1
    $KeyLocation = $builder.Uri
    Write-Verbose "KeyLocation not provided. Defaulting to $($KeyLocation.AbsoluteUri)"
}

if ($KeyLocation.Host -ne $hostName) {
    throw "KeyLocation ($($KeyLocation.AbsoluteUri)) must share the same host as the URLs ($hostName)."
}

$urlTotal = $uriList.Count
Write-Information "Submitting $urlTotal URLs to $($Endpoint.AbsoluteUri)"

for ($offset = 0; $offset -lt $uriList.Count; $offset += $UrlsPerRequest) {
    $batchSize = [Math]::Min($UrlsPerRequest, $uriList.Count - $offset)
    $chunkUris = $uriList.GetRange($offset, $batchSize)

    $payload = [ordered]@{
        host = $hostName
        key = $Key
        urlList = $chunkUris | ForEach-Object { $_.AbsoluteUri }
    }

    if ($KeyLocation) {
        $payload.keyLocation = $KeyLocation.AbsoluteUri
    }

    $jsonPayload = $payload | ConvertTo-Json -Depth 5
    $rangeText = "URL $($offset + 1)-$($offset + $batchSize) ($batchSize item(s))"

    if ($DryRun) {
        Write-Information "[DryRun] Would submit $rangeText to $($Endpoint.AbsoluteUri)"
        Write-Information $jsonPayload
        continue
    }

    if (-not $PSCmdlet.ShouldProcess($Endpoint.AbsoluteUri, "Submit $rangeText")) {
        Write-Information "Skipping $rangeText (WhatIf mode)"
        continue
    }

    Write-Information "Submitting $rangeText to $($Endpoint.AbsoluteUri)"

    $response = Invoke-WebRequest -Uri $Endpoint -Method Post -ContentType 'application/json; charset=utf-8' -Body $jsonPayload -ErrorAction Stop
    $statusCode = [int]$response.StatusCode

    if ($statusCode -ge 200 -and $statusCode -lt 300) {
        Write-Information "Submission succeeded: HTTP $statusCode $($response.StatusDescription)"
    }
    else {
        $bodyPreview = $response.Content
        throw "IndexNow returned status code ${statusCode}: $bodyPreview"
    }
}

Write-Information 'IndexNow submission complete.'
