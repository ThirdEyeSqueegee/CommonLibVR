# CommonLibSSE-NG

Fork of a fork of a fork of a fork of a...

## Usage

Add the following to `vcpkg.json`:

```json
"dependencies": ["commonlibsse-ng", ...],
// or
// "dependencies": [{ "name": "commonlibsse-ng", "features": ["xbyak"] }, ...]
...
"vcpkg-configuration": {
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG-vcpkg",
      "baseline": "<hash of the latest commit in the above repo>",
      "packages": ["commonlibsse-ng", "spdlog"]
    }
  ],
}
```

## History

[Ryan-rsm-McKenzie/CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) -> [powerof3/CommonLibSSE](https://github.com/powerof3/CommonLibSSE) -> [CharmedBaryon/CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) -> [alandtse/CommonLibVR/ng](https://github.com/alandtse/CommonLibVR/tree/ng) -> [this repo](#)
