## File Structure

Here I have created four separate "projects", one for each of the Arduinos. The `/utils` directory should contain header files that contain definitions (🤮) for common functions like packet communication, type definitions, etc. These header files then can be included in the "main" `.ino` file in each project.

Because of how the build process is so effed, (stuff gets moved to a temp directory), header files outside of the project directory can't actually be included, so there is a symlink to the `utils` folder in each of the projects. See [docs](https://arduino.github.io/arduino-cli/1.2/sketch-build-process/#pre-processing).

Let's keep it clean out there 💪.

Just as a note, the `sketch.yaml` in each project may have to be updated for your system, we can `git rm` if this is an issue, I'm not 100%
