from conans.model.conan_file import ConanFile
from conans import CMake
import os

class TracesTestPackage(ConanFile):
    generators = 'cmake'

    def build(self):
        cmake = CMake(self, parallel=False)
        cmake.verbose = True
        cmake.configure()
        cmake.build()

    def test(self):
        self.run(os.path.join('bin', 'traces_example'))
