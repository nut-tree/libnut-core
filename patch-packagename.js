const fs = require("fs");
const { join } = require("path");

if (process.env.CI) {
  const filename = join(__dirname, "package.json");
  const packageJson = require(filename);

  const plattformPackageName = `@nut-tree/libnut-${process.platform}`;
  packageJson.name = plattformPackageName;

  try {
    fs.writeFileSync(filename, JSON.stringify(packageJson, null, 2));
  } catch (err) {
    console.log(err);
    process.exit(-1);
  }
  console.log(`Patched package name to '${plattformPackageName}'`);
}
