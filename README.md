# C Programming — From Scratch to Advanced

A complete, beginner-friendly tutorial on the C programming language — written for someone who has **never programmed before** and taken all the way through to advanced, professional C topics (pointers, dynamic memory, data structures, multi-file builds, and debugging).

This repository is structured as an [MkDocs](https://www.mkdocs.org/) site (using the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) theme) and is meant to be published as a static site via **GitHub Pages**, deployed from a branch (the standard `mkdocs gh-deploy` workflow, same as the `system-engineering` repo this project mirrors).

---

## 🌐 Language versions

The tutorial now has a language entry point for learners who prefer English or हिंदी. The C code remains in standard English syntax, while explanations can be localized.

- English: `docs/index.md`
- हिंदी: `docs/hi/index.md`

Hindi translated chapters are available for Chapter 0 through Chapter 20. Continue translating appendices using the same style: keep technical words like `variable`, `function`, `array`, `pointer`, `compiler`, and `memory` in English, then explain them in Hindi.

---

## 📖 What's inside

- **Getting Started** — what programming is, what C is, installing a compiler and editor, writing and compiling your first program.
- **Language Basics** — variables, data types, operators, input/output, decision making, loops.
- **Core Concepts** — functions, arrays, strings, pointers, structures/unions/enums.
- **Intermediate Topics** — dynamic memory allocation, file handling, the preprocessor, storage classes, recursion.
- **Advanced Topics** — advanced pointer techniques, classic data structures (linked lists, stacks, queues, trees) implemented in C, multi-file projects with Makefiles, debugging and best practices.
- **Appendices** — a standard library quick reference, cheat sheets (operator precedence, format specifiers, ASCII table), and practice project ideas.

Every chapter is self-contained, includes runnable code samples with expected output, and ends with practice exercises.

---

## 🛠 Prerequisites

You need two completely separate things to work with this project:

1. **A C compiler** — to actually write and run the C programs taught in the tutorial.
2. **Python + MkDocs** — to build/preview/deploy the *documentation website* itself.

You do **not** need Python or MkDocs to learn C from these notes — you could just read the Markdown files on GitHub. MkDocs is only needed if you want to build/serve the polished website locally or deploy it.

### 1. Installing a C compiler (for following the tutorial)

#### Ubuntu / Debian / WSL

```bash
sudo apt update
sudo apt install build-essential gdb
gcc --version
```

#### Fedora

```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install gdb
gcc --version
```

#### macOS

```bash
xcode-select --install
gcc --version   # actually clang under the hood, that's fine
```

#### Windows

Install [MSYS2](https://www.msys2.org/) and then, in the MSYS2 terminal:

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain gdb
```

Or install [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) and follow the Ubuntu instructions above (recommended for beginners).

A code editor is also recommended — [VS Code](https://code.visualstudio.com/) with the **C/C++** extension (by Microsoft) works well on every platform.

Full, detailed setup instructions (with screenshots-in-words, troubleshooting tips, and how to compile/run your first program) are in [Chapter 1 — Setting Up Your Environment](docs/chapters/chapter-01.md).

### 2. Installing MkDocs (for building/previewing this website)

Requires Python 3.8+.

```bash
# (Recommended) create an isolated virtual environment first
python3 -m venv .venv
source .venv/bin/activate        # On Windows: .venv\Scripts\activate

# Install MkDocs, the Material theme, and the extensions used by this site
pip install mkdocs mkdocs-material pymdown-extensions
```

Or, if you prefer to pin everything in a `requirements.txt`:

```text
mkdocs
mkdocs-material
pymdown-extensions
```

```bash
pip install -r requirements.txt
```

---

## ▶️ Running the site locally

From the root of this repository (where `mkdocs.yml` lives):

```bash
mkdocs serve
```

This starts a local dev server (by default at `http://127.0.0.1:8000/`) with **live reload** — edit any Markdown file in `docs/` and the browser refreshes automatically.

To just build the static site into the `site/` folder (without serving it):

```bash
mkdocs build
```

---

## 🚀 Deploying to GitHub Pages (deploy from a branch)

MkDocs ships a one-command deploy helper that builds the site and pushes it to a `gh-pages` branch, which you then point GitHub Pages at.

1. Make sure your repository is pushed to GitHub and `site_url` in `mkdocs.yml` matches your GitHub Pages URL (e.g. `https://<your-username>.github.io/<repo-name>/`).
2. Run:
   ```bash
   mkdocs gh-deploy --force
   ```
   This builds the site and force-pushes the result to a `gh-pages` branch on `origin`.
3. On GitHub: go to **Settings → Pages**, set **Source** to **Deploy from a branch**, and select the `gh-pages` branch (folder `/ (root)`). Save.
4. After a minute or two, your site will be live at the URL shown on that settings page.

From then on, whenever you update the docs, just re-run `mkdocs gh-deploy --force` to publish the changes.

> Alternative: you can instead set up a GitHub Actions workflow (`.github/workflows/deploy.yml`) that runs `mkdocs gh-deploy` automatically on every push to `main`. The manual command above is the simplest starting point and is what this README assumes.

---

## 📂 Project structure

```text
Basic_C_to_advanced_C/
├── mkdocs.yml                # Site configuration (nav, theme, plugins)
├── README.md                 # This file
└── docs/
    ├── index.md               # Home page
    ├── about.md                # About page
    ├── assets/                 # Images, logo
    ├── stylesheets/
    │   └── extra.css           # Custom theme tweaks
    └── chapters/
        ├── chapter-00.md ... chapter-20.md
        └── Appendix-A.md, Appendix-B.md, Appendix-C.md
```

---

## 🤝 Using these notes

Read the chapters in order if you are new to programming — each one builds on the last. If you already know another programming language, skim Chapters 0–2 and start from Chapter 3, or jump directly to whichever topic you need from the navigation menu.

Every code example in this tutorial is written in standard **C11**, compiled and tested with `gcc`. Where a feature is compiler- or platform-specific, the text calls it out explicitly.
