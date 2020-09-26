function Pre-Release {
    $timestamp = Get-Date -Format yyyyMMddhhmmss
    $patchVersion = (npm --no-git-tag version patch)
    $nextVersion = "${patchVersion}-next.${timestamp}".Substring(1)
    echo $nextVersion

    npm version --no-git-tag -f $nextVersion
    npm run publish:next
}

function Release {
    npm publish
}

if ($env:APPVEYOR_REPO_BRANCH -eq "develop") {
    Pre-Release
} elseif ($env:APPVEYOR_REPO_TAG -eq "true") {
    Release
}