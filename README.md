# ARTist - The Android Runtime instrumentation and security toolkit
   
[![Gitter](https://badges.gitter.im/Project-ARTist/meta.svg)](https://gitter.im/project-artist/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=body_badge)

ARTist is a flexible open source instrumentation framework for Android's apps and Java middleware. It is based on the Android Runtime’s (ART) compiler and modifies code during on-device compilation. In contrast to existing instrumentation frameworks, it preserves the application's original signature and operates on the instruction level. 

This repository represents the core of the ARTist ecosystem, the actual implementation of the instrumentation framework. It is integrated into AOSP's [Android Runtime (ART)](https://github.com/Project-ARTist/art) project to create a specialized version of the on-device ```dex2oat``` compiler and  provides developers with a Module API that gives full control over the target's complete Java source code, provided in the compiler's intermediate representation. ARTist allows end users to utilize already available Modules, developers to create own instrumentations for apps and the Android system, and researchers to analyze and extend app and system behavior. 

ARTist can be deployed in two different ways: First, as a regular application using our [ArtistGui](https://github.com/Project-ARTist/ArtistGui) project that allows for non-invasive app instrumentation on rooted devices, or second, as a system compiler for custom ROMs where it can additionally instrument the system server (Package Manager Service, Activity Manager Service, ...) and the Android framework classes (```boot.oat```). It supports Android versions after (and including) Marshmallow 6.0. 

For detailed tutorials and more in-depth information on the ARTist ecosystem, have a look at our [official documentation](https://artist.cispa.saarland) and join our [Gitter chat](https://gitter.im/project-artist/Lobby).

## Version Independence

You may find guards in the code that check for particular Android versions since some APIs we use internally may have changed over versions. The ART project is changing steadily, so we try to compartmentalize all version-dependent code either in our [ART fork](https://github.com/Project-ARTist/art) or hide it behind more convenient, version-independent APIs.
The overall goal is to hide the version-dependence from module developers so that the resulting modules are compatible with all Android releases supported by ARTist. 

## Writing Modules

In the current pre-beta version, modules are created under the ```modules``` folder and compiled with ARTist. However, our upcoming beta release (see below) will feature a dedicated SDK that allows to develop modules independently from the ARTist source code, hence no AOSP is needed anymore and you do not need to hardcode modules but can choose which ones to use for a particular target dynamically.

## Building ARTist

If you just want to create own instrumentations, you do not need to bother about building ARTist. Just stick with the explanation above. If you, however, want to work on the instrumentation framework itself, the following will help you to get started: 

ARTist is an extension to the ART compiler ```dex2oat```, hence it is embedded as a submodule in our customized [ART fork](https://github.com/Project-ARTist/art). In order to build ARTist, you need to build the ART fork as a part of AOSP. What we refer to as the ARTist version of ```dex2oat``` is actually the ```dex2oat``` binary plus several libraries (e.g., ```libart-compiler```) that together form our instrumenting compiler. The  [ArtistGui](https://github.com/Project-ARTist/ArtistGui) project has ready-made scripts to build ART and ARTist, and copy the resulting binaries & libs into the correct folders of ArtistGui to ship them to a device for testing. 


## Upcoming Beta Release

We are about to enter the beta phase soon, which will bring a lot of changes to the whole ARTist ecosystem, including a dedicated ARTist SDK for simplified Module development, a semantic versioning-inspired release and versioning scheme, an improved and updated version of our online documentation, great new Modules, and a lot more improvements. However, in particular during the transition phase, some information like the one in the repositories' README.md files and the documentation at [https://artist.cispa.saarland](https://artist.cispa.saarland) might be slightly out of sync. We apologize for the inconvenience and happily take feedback at [Gitter](https://gitter.im/project-artist/Lobby). To keep up with the current progress, keep an eye on the beta milestones of the Project: ARTist repositories and check for new blog posts at [https://artist.cispa.saarland](https://artist.cispa.saarland) . 

## Contribution

We hope to create an active community of developers, researchers and users around Project ARTist and hence are happy about contributions and feedback of any kind. There are plenty of ways to get involved and help the project, such as testing and writing Modules, providing feedback on which functionality is key or missing, reporting bugs and other issues, or in general talk about your experiences. The team is actively monitoring [Gitter](https://gitter.im/project-artist/) and of course the repositories, and we are happy to get in touch and discuss. We do not have a full-fledged contribution guide, yet, but it will follow soon (see beta announcement above). 

## Academia

ARTist is based on a paper called **ARTist - The Android Runtime Instrumentation and Security Toolkit**, published at the 2nd IEEE European Symposium on Security and Privacy (EuroS&P'17). The full paper is available [here](https://artist.cispa.saarland/res/papers/ARTist.pdf). If you are citing ARTist in your research, please use the following bibliography entry:

```
@inproceedings{artist,
  title={ARTist: The Android runtime instrumentation and security toolkit},
  author={Backes, Michael and Bugiel, Sven and Schranz, Oliver and von Styp-Rekowsky, Philipp and Weisgerber, Sebastian},
  booktitle={2017 IEEE European Symposium on Security and Privacy (EuroS\&P)},
  pages={481--495},
  year={2017},
  organization={IEEE}
}
```

There is a follow-up paper where we utilized ARTist to cut out advertisement libraries from third-party applications, move the library to a dedicated app (own security principal) and reconnect both using a custom Binder IPC protocol, all while preserving visual fidelity by displaying the remote advertisements as floating views on top of the now ad-cleaned application. The full paper **The ART of App Compartmentalization: Compiler-based Library Privilege Separation on Stock Android**, as it was published at the 2017 ACM SIGSAC Conference on Computer and Communications Security (CCS'17), is available [here](https://artist.cispa.saarland/res/papers/CompARTist.pdf).
