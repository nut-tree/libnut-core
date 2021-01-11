"//registry.npmjs.org/:_authToken=$env:NPM_TOKEN`n" | out-file "$env:userprofile\.npmrc" -Encoding ASCII
npm whoami

$timestamp = Get-Date -Format yyyyMMddhhmmss
$patchVersion = (npm --no-git-tag version patch)
$nextVersion = "${patchVersion}-next.${timestamp}".Substring(1)
echo $nextVersion

npm version --no-git-tag -f $nextVersion
npm run publish:next