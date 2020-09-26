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
    echo "Running pre-release on branch $env:APPVEYOR_REPO_BRANCH"
    Pre-Release
} elseif ($env:APPVEYOR_REPO_TAG -eq "true") {
    echo "Running release on tag $env:APPVEYOR_REPO_TAG_NAME"
    Release
}