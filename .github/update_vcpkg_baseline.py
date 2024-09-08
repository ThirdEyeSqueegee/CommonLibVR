from json import dump, load
from re import sub
from subprocess import run

vcpkg_repo = "https://github.com/microsoft/vcpkg"
clib_vcpkg_registry_repo = "https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG-vcpkg"

vcpkg_ref = (
    run(["git", "ls-remote", vcpkg_repo, "refs/heads/master"], capture_output=True)
    .stdout.decode()
    .splitlines()[0]
    .split()[0]
)

clib_vcpkg_registry_ref = (
    run(["git", "ls-remote", clib_vcpkg_registry_repo], capture_output=True)
    .stdout.decode()
    .splitlines()[0]
    .split()[0]
)

with open("../../vcpkg.json", "r") as f:
    vcpkg_json = load(f)

vcpkg_json["builtin-baseline"] = vcpkg_ref
vcpkg_json["vcpkg-configuration"]["registries"][0]["baseline"] = clib_vcpkg_registry_ref

with open("../../vcpkg.json", "w") as f:
    dump(vcpkg_json, f, indent=2)

with open("./workflows/main_ci.yml", "r") as f:
    main_ci_yml = f.read()

main_ci_yml = sub(
    r"(VCPKG_COMMIT_ID: ).*", f"VCPKG_COMMIT_ID: {vcpkg_ref}", main_ci_yml
)

with open("./workflows/main_ci.yml", "w") as f:
    f.write(main_ci_yml)
