# AED Simulator
This Qt C++ program simulates the interface and functionality of an [AED+](https://www.zoll.com/products/aeds/aeds-for-public-access/aed-plus-for-public-access) Automated External Defibrillator device.

This was originally created as a course project for _COMP 3004: Object-Oriented Software Engineering_, Carleton University, 2023.

Team members: Gerry Huynh, Chari Idnani, Shariat Jahan Shanu, Shaun Sim, Nic Watson

![aed](https://github.com/nicwatson/aed-project/assets/587252/f779aac5-0f52-4300-8173-e46197d5b7b1)


## Files and file structure

   - `AED.pro`: The project file.
   - `aedModel`, `aedGui`, `event`: Directories containing the backend and frontend code for the AED project.
   - `mainwindow.h`, `mainwindow.cpp`, `mainwindow.ui`: Files related to the main window of the application.
   - `qcustomplot.h`, `qcustomplot.cpp`: Files for the custom plot functionality.
   - `LICENSE`: The license file for the project.
   - `aedconst.h`: A header file containing constants used in the project.
   - `main.cpp`: The main file that runs the application.
   - `assets`: A directory containing assets used in the project.

## Contributions/Credeits

1. Gerry Huynh
   - Use cases
   - GUI (including SVGs)
   - `LCDDisplay` class (backend)
2. Charu Idnani
   - Use Cases
   - Traceability matrix
   - Sequence Diagram for normal success scenario
   - Video demo
3. Shariat Jahan Shanu
   - Use Cases
   - Design Documentaion
   - UML class digram
   - Sequence Diagrams for safety scenarios
   - README.md
4. Shaun Sim
   - Use cases
   - Middleware
   - Some backend (`ModuleECGAssessment`)
   - Requirements clarification with client (Voja)
5. Nic Watson
   - State diagrams
   - Backend
   - Class design: `AED`, `ModuleSelfTest`, `ModuleStartupAdvice`, `ModuleShock`, `ModuleCPRHelp`
   - CPR backend logic classes (`CPRHelpStrategy` etc.)
   - `LampWidget`/`LampButton`
   - Signal/slot and sequence tracing/documentation
