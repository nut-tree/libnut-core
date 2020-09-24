$timestamp = Get-Date -Format yyyyMMddhhmmss
$patchVersion = (npm --no-git-tag version patch)
$nextVersion = "${patchVersion}-next.${timestamp}".Substring(1)
echo $nextVersion

npm version --no-git-tag -f $nextVersion
npm run publish:next
