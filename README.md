# COMP3004
Repo for the development work on the Comp 3004 group project for Team 14


# Team Members
1. Gerry Huynh
   - Use cases
   - GUI (including SVGs)
   - `LCDDisplay` class (backend)
3. Charu Idnani
   - Traceability matrix
   - Sequence Diagram for normal success scenario
   - Video
4. Shariat Jahan Shanu
   - Design Documentaion
   - UML class digram
   - Sequence Diagrams for safety scenarios
5. Shaun Sim
   - Use cases
   - Middleware
   - Some backend (`ModuleECGAssessment`)
   - Requirements clarification with client (Voja)
6. Nic Watson
   - State diagrams
   - Backend
   - Class design: `AED`, `ModuleSelfTest`, `ModuleStartupAdvice`, `ModuleShock`, `ModuleCPRHelp`
   - CPR backend logic classes (`CPRHelpStrategy` etc.)
   - `LampWidget`/`LampButton`
   - Signal/slot and sequence tracing/documentation

# Organization of the submission

1. **Documentation**: This directory contains all the documents related to the design and development of the project. It includes:

   - `AED State Diagram.png`: The state diagram for the AED.
   - `AED UML.png`: The UML class diagram for the AED project.
   - **Sequence Diagrams**:This directory contains all the sequence diagrams created for the project. It includes:

         - `Main Success Scenario Diagram.png`: The sequence diagram for the main success scenario of the AED project.

         - **Safety Scenarios**: A sub-directory containing sequence diagrams for various safety scenarios. It includes:
         - `Cable unplugged.png`: The sequence diagram for the scenario where the cable is unplugged during a current session.
         - `Self test fail.png`: The sequence diagram for the scenario where the self-test fails.
         - `Low battery.png`: The sequence diagram for the scenario where a low battery condition is reached during operation.
         - `Unattached pads.png`: The sequence diagram for the scenario where the electrode pads are unattached from the AED during a current session.
  
   - `Textual explanation of design decisions.pdf`: A document explaining the design decisions made during the development of the project.
   - `Traceability Matrix.pdf`: The traceability matrix for the project.
   - `Video`: A video demonstration of the project.

2. **Project code**: This directory contains all the source code files for the AED project. It includes:

   - `AED.pro`: The project file.
   - `aedModel`, `aedGui`, `event`: Directories containing the backend and frontend code for the AED project.
   - `mainwindow.h`, `mainwindow.cpp`, `mainwindow.ui`: Files related to the main window of the application.
   - `qcustomplot.h`, `qcustomplot.cpp`: Files for the custom plot functionality.
   - `LICENSE`: The license file for the project.
   - `aedconst.h`: A header file containing constants used in the project.
   - `main.cpp`: The main file that runs the application.
   - `assets`: A directory containing assets used in the project.
