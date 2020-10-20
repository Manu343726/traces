from conans import python_requires, tools
from conans.errors import ConanException
import os, re

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

class Git(tools.Git):

    def get_last_tag(self):
        self.check_repo()
        try:
            status = self.run("describe --abbrev=0")
            tag = status.strip()
            return tag
        except Exception:
            return None

def get_tag_version(git):
    tag = git.get_tag()

    if tag is None:
        return None

    result = re.fullmatch(r'v([0-9]+)\.([0-9]+)\.([0-9]+)(?:\-([0-9]+))', tag)

    if result is None:
        raise ConanException('Invalid git tag "{}"'.format(tag))

    if len(result.groups()) > 3:
        return result.expand(r'\1.\2.\3-\4')
    else:
        return result.expand(r'\1.\2.\3')

class Traces(common.CMakePackage):
    name = 'traces'
    url = 'https://github.com/Manu343726/traces'
    description = 'Tracy traces on steroids'
    license = 'MIT'
    scm = {
        'type': 'git',
        'url': 'https://github.com/Manu343726/traces',
        'revision': 'auto',
        'subfolder': 'traces'
    }
    generators = 'cmake_find_package'
    requires = ('tracy_client/master',
                'fmt/6.2.0')
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {
        'TRACES_DISABLE_STACK_TRACES': [True, False],
        'TRACES_DISABLE_MEMORY_ALLOCATION_TRACES': [True, False],
        'TRACES_DISABLE_MUTEX_TRACES': [True, False],
    }
    default_options = {
        'tracy_client:TRACY_ENABLE': True,
        'tracy_client:TRACY_ON_DEMAND': True,
        'tracy_client:TRACY_NO_EXIT': False,
        'TRACES_DISABLE_STACK_TRACES': False,
        'TRACES_DISABLE_MEMORY_ALLOCATION_TRACES': False,
        'TRACES_DISABLE_MUTEX_TRACES': False
    }

    def set_version(self):
        git = Git(self.recipe_folder)
        tag_version = get_tag_version(git)

        if tag_version is not None:
            self.version = tag_version
        else:
            self.version = '{}-{}'.format(git.get_branch(), git.get_commit())

    def package_info(self):
        self.cpp_info.libs = ['traces']

        if self.options.TRACES_DISABLE_STACK_TRACES:
            self.cpp_info.defines.append('TRACES_DISABLE_STACK_TRACES')

        if self.options.TRACES_DISABLE_MEMORY_ALLOCATION_TRACES:
            self.cpp_info.defines.append('TRACES_DISABLE_MEMORY_ALLOCATION_TRACES')

        if self.options.TRACES_DISABLE_MUTEX_TRACES:
            self.cpp_info.defines.append('TRACES_DISABLE_MUTEX_TRACES')
