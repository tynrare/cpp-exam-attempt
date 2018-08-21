### Installation ###
```bash
mkdir build
cd build
cmake .. && make
```
You may need to use`sudo make install` command as it will install to /usr/local/bin .  This might be a good opportunity to investigate stow (http://www.gnu.org/software/stow/manual/stow.html) as a means of managing the installation of applications in this manner, however I haven't gotten far enough into my CMake knowledge to take that step.
