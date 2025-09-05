# Jarch

Jarch is a custom CPU architecture project under Clouden Blackline, including tools, emulators, and projects like kernels and missile systems. This repository is structured to separate development tools from actual projects.

> **Note:** All numbers and digits in both source code and compiled binaries are represented in **hexadecimal**.

## About

This is my first GitHub repository, and I would greatly appreciate any feedback or critique specifically on the CPU-related files and architecture (Jarch itself). This includes the assembler, emulator, instruction set, or any design decisions in the CPU and Jcode. Constructive criticism is welcome, especially regarding organization, efficiency, or clarity of the CPU files.

## Repository Structure

- **Tools/src/** – Development tools for the Jarch architecture:
  - **Assembler** – Transpiles Jcode into executable instructions.  
  - **Emulator** – Runs compiled Jcode programs and provides debugging utilities.  
  - **BinViewer** – Displays only memory addresses in the final binary that contain non-zero values.  

- **Projects/** – Contains code for specific projects built on Jarch, such as:
  - **Kernel1** – Experimental OS kernel  
  - **MissileSystem** – Simulated missile control system

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/CloudenBlackLine/Jarch.git
