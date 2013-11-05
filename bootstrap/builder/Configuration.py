
# -*- mode:Python -*-
#
# This file is part of the Coriolis Software.
# Copyright (c) UPMC/LIP6 2008-2013, All Rights Reserved
#
# +-----------------------------------------------------------------+ 
# |                   C O R I O L I S                               |
# |    C o r i o l i s  /  C h a m s   B u i l d e r                |
# |                                                                 |
# |  Author      :                   Jean-Paul Chaput               |
# |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
# | =============================================================== |
# |  Python      :   "./builder/Configuration.py"                   |
# +-----------------------------------------------------------------+


import sys
import re
import os
import os.path
import datetime
import subprocess
from   .       import ErrorMessage
from   Project import Project


class Configuration ( object ):

    PrimaryNames = \
        [ 'confFile', 'projects', 'standalones'
        , 'svnTag', 'svnMethod'
        , 'projectDir', 'rootDir'
        , 'packageName', 'packageVersion', 'packageExcludes', 'packageProject'
        , 'osType', 'libSuffix', 'buildMode', 'libMode', 'enableShared'
        ]
    SecondaryNames = \
        [ 'rpmbuildDir' , 'debbuildDir'   , 'tmppathDir'  , 'tarballDir'
        , 'archiveDir'  , 'sourceDir'     , 'osDir'       , 'buildDir'
        , 'installDir'  , 'specFileIn'  , 'specFile'
        , 'debianDir'   , 'debChangelogIn', 'debChangelog', 'sourceTarBz2'
        , 'binaryTarBz2', 'distribPatch'
        ]

    def __init__ ( self ):
        self._confFile         = None
        self._projects         = []
        self._standalones      = []
        self._svnTag           = "x"
        self._svnMethod        = None
        self._projectDir       = 'coriolis-2.x'
        self._rootDir          = os.path.join ( os.environ["HOME"], self._projectDir )
        self._packageName      = None
        self._packageVersion   = None
        self._packageExcludes  = []
        self._packageProject   = []
        self._enableShared     = 'ON'
        self._buildMode        = 'Release'

       # Secondary (derived) variables.
       # Setup by _updateSecondary().
        self._guessOs()
        self._updateSecondary()
        return


    def __setattr__ ( self, attribute, value ):
        if attribute in Configuration.SecondaryNames:
            print ErrorMessage( 1, 'Attempt to write in read-only attribute <%s> in Configuration.'%attribute )
            return

        if attribute[0] == '_':
            self.__dict__[attribute] = value
            return

        if   attribute == 'rootDir': value = os.path.expanduser(value)
        elif attribute == 'enableShared' and value != 'ON': value = 'OFF'

        self.__dict__['_'+attribute] = value
        self._updateSecondary()
        return


    def __getattr__ ( self, attribute ):
        if attribute[0] != '_': attribute = '_'+attribute
        if not self.__dict__.has_key(attribute):
            raise ErrorMessage( 1, 'Configuration has no attribute <%s>.'%attribute )
        return self.__dict__[attribute]


    def _updateSecondary ( self ):
        if self._enableShared == "ON": self._libMode = "Shared"
        else:                          self._libMode = "Static"

       #self._rootDir     = os.path.join ( os.environ["HOME"], self._projectDir )
        self._rpmbuildDir = os.path.join ( self._rootDir    , "rpmbuild" )
        self._debbuildDir = os.path.join ( self._rootDir    , "debbuild" )
        self._tmppathDir  = os.path.join ( self._rpmbuildDir, "tmp" )
        self._tarballDir  = os.path.join ( self._rootDir    , "tarball" )
        self._archiveDir  = os.path.join ( self._tarballDir , "%s-%s.%s" % (self._packageName
                                                                           ,self._packageVersion
                                                                           ,self._svnTag) )
        self._sourceDir   = os.path.join ( self._rootDir    , "src" )
        self._osDir       = os.path.join ( self._rootDir
                                         , self._osType
                                         , "%s.%s" % (self._buildMode,self._libMode) )
        self._buildDir    = os.path.join ( self._osDir, "build" )
        self._installDir  = os.path.join ( self._osDir, "install" )

        self._specFileIn     = os.path.join ( self._sourceDir, "bootstrap", "%s.spec.in"%self._packageName )
        self._specFile       = os.path.join ( self._sourceDir, "bootstrap", "%s.spec"   %self._packageName )
        self._debianDir      = os.path.join ( self._sourceDir, "bootstrap", "debian" )
        self._debChangelogIn = os.path.join ( self._debianDir, "changelog.in" )
        self._debChangelog   = os.path.join ( self._debianDir, "changelog" )
        self._sourceTarBz2   = "%s-%s.%s.tar.bz2"                 % (self._packageName,self._packageVersion,self._svnTag)
        self._binaryTarBz2   = "%s-binary-%s.%s-1.slsoc6.tar.bz2" % (self._packageName,self._packageVersion,self._svnTag)
        self._distribPatch   = os.path.join ( self._sourceDir, "bootstrap", "%s-for-distribution.patch"%self._packageName )
        return


    def _guessOs ( self ):
        self._libSuffix         = None
        self._osSlsoc6x_64      = re.compile (".*Linux.*(el6|slsoc6).*x86_64.*")
        self._osSlsoc6x         = re.compile (".*Linux.*(el6|slsoc6).*")
        self._osSLSoC5x_64      = re.compile (".*Linux.*el5.*x86_64.*")
        self._osSLSoC5x         = re.compile (".*Linux.*(el5|2.6.23.13.*SoC).*")
        self._osLinux_64        = re.compile (".*Linux.*x86_64.*")
        self._osLinux           = re.compile (".*Linux.*")
        self._osFreeBSD8x_amd64 = re.compile (".*FreeBSD 8.*amd64.*")
        self._osFreeBSD8x_64    = re.compile (".*FreeBSD 8.*x86_64.*")
        self._osFreeBSD8x       = re.compile (".*FreeBSD 8.*")
        self._osDarwin          = re.compile (".*Darwin.*")

        uname = subprocess.Popen ( ["uname", "-srm"], stdout=subprocess.PIPE )
        lines = uname.stdout.readlines()

        if self._osSlsoc6x_64.match(lines[0]):
            self._osType    = "Linux.slsoc6x_64"
            self._libSuffix = "64"
        elif self._osSlsoc6x   .match(lines[0]): self._osType = "Linux.slsoc6x"
        elif self._osSLSoC5x_64.match(lines[0]):
            self._osType    = "Linux.SLSoC5x_64"
            self._libSuffix = "64"
        elif self._osSLSoC5x   .match(lines[0]): self._osType = "Linux.SLSoC5x"
        elif self._osLinux_64  .match(lines[0]):
            self._osType    = "Linux.x86_64"
            self._libSuffix = "64"
        elif self._osLinux     .match(lines[0]): self._osType = "Linux.i386"
        elif self._osDarwin    .match(lines[0]): self._osType = "Darwin"
        elif self._osFreeBSD8x_amd64.match(lines[0]):
            self._osType    = "FreeBSD.8x.amd64"
            self._libSuffix = "64"
        elif self._osFreeBSD8x_64.match(lines[0]):
            self._osType    = "FreeBSD.8x.x86_64"
            self._libSuffix = "64"
        elif self._osFreeBSD8x .match(lines[0]): self._osType = "FreeBSD.8x.i386"
        else:
            uname = subprocess.Popen ( ["uname", "-sr"], stdout=subprocess.PIPE )
            self._osType = uname.stdout.readlines()[0][:-1]

            print "[WARNING] Unrecognized OS: \"%s\"." % lines[0][:-1]
            print "          (using: \"%s\")" % self._osType
        
        return


    def getPrimaryIds   ( self ): return Configuration.PrimaryNames
    def getSecondaryIds ( self ): return Configuration.SecondaryNames
    def getAllIds       ( self ): return Configuration.PrimaryNames + Configuration.SecondaryNames


    def register ( self, project ):
        for registered in self._projects:
            if registered.getName() == project.getName():
                print ErrorMessage( 0, "Project \"%s\" is already registered (ignored)." )
                return
        self._projects += [ project ]
        return


    def getProject ( self, name ):
        for project in self._projects:
            if project.getName() == name:
                return project
        return None


    def getToolProject ( self, name ):
        for project in self._projects:
            if project.hasTool(name):
                return project
        return None


    def load ( self, confFile ):
        moduleGlobals = globals()

        if not confFile:
            print 'Making an educated guess to locate the configuration file:'
            locations = [ os.path.abspath(os.path.dirname(sys.argv[0]))
                        , os.environ['HOME']+'/coriolis-2.x/src/bootstrap'
                        , os.environ['HOME']+'/coriolis/src/bootstrap'
                        , os.environ['HOME']+'/chams-2.x/src/bootstrap'
                        , os.environ['HOME']+'/chams/src/bootstrap'
                        , '/users/outil/coriolis/coriolis-2.x/src/bootstrap'
                        , self._rootDir+'/src/bootstrap'
                        ]

            for location in locations:
                self._confFile = location + '/build.conf'
                print '  <%s>' % self._confFile

                if os.path.isfile(self._confFile): break
            if not self._confFile:
                ErrorMessage( 1, 'Cannot locate any configuration file.' ).terminate()
        else:
            print 'Using user-supplied configuration file:'
            print '  <%s>' % confFile

            self._confFile = confFile
            if not os.path.isfile(self._confFile):
                ErrorMessage( 1, 'Missing configuration file:', '<%s>'%self._confFile ).terminate()

        print 'Reading configuration from:'
        print '  <%s>' % self._confFile
        
        try:
            execfile( self._confFile, moduleGlobals )
        except Exception, e:
            ErrorMessage( 1, 'An exception occured while loading the configuration file:'
                           , '<%s>\n' % (self._confFile)
                           , 'You should check for simple python errors in this file.'
                           , 'Error was:'
                           , '%s\n' % e ).terminate()
        
        if moduleGlobals.has_key('projects'):
            entryNb = 0
            for entry in moduleGlobals['projects']:
                entryNb += 1
                if not entry.has_key('name'):
                    raise ErrorMessage( 1, 'Missing project name in project entry #%d.' % entryNb )
                if not entry.has_key('tools'):
                    raise ErrorMessage( 1, 'Missing tools list in project entry #%d (<%s>).' \
                                           % (entryNb,entry['name']) )
                if not isinstance(entry['tools'],list):
                    raise ErrorMessage( 1, 'Tools item of project entry #%d (<%s>) is not a list.' \
                                           % (entryNb,entry['name']) )
                if not entry.has_key('repository'):
                    raise ErrorMessage( 1, 'Missing project repository in project entry #%d.' \
                                           % entryNb )

                self.register( Project(entry['name'],entry['tools'],entry['repository']) )
        else:
            ErrorMessage( 1, 'Configuration file is missing the \'project\' symbol.'
                           , '<%s>'%self._confFile ).terminate()

        if moduleGlobals.has_key('projectdir'):
            self.projectDir = moduleGlobals['projectdir']

        if moduleGlobals.has_key('svnconfig'):
            svnconfig = moduleGlobals['svnconfig']
            if svnconfig.has_key('method'): self._svnMethod = svnconfig['method']

        if moduleGlobals.has_key('package'):
            package = moduleGlobals['package']
            if package.has_key('name'    ): self.packageName    = package['name']
            if package.has_key('version' ): self.packageVersion = package['version']
            if package.has_key('excludes'):
                if not isinstance(package['excludes'],list):
                    raise ErrorMessage( 1, 'Excludes of package configuration is not a list.')
                self._packageExcludes = package['excludes']
            if package.has_key('projects'):
                if not isinstance(package['projects'],list):
                    raise ErrorMessage( 1, 'Projects to package is not a list.')
                self._packageProjects = package['projects']
        return


    def show ( self ):
        print 'CCB Configuration:'
        if self._svnMethod:
            print '  SVN Method: <%s>' % self._svnMethod
        else:
            print '  SVN Method not defined, will not be able to checkout/commit.'

        for project in self._projects:
            print '  project:%-15s repository:<%s>' % ( ('<%s>'%project.getName()), project.getRepository() )
            toolOrder = 1
            for tool in project.getTools():
                print '%s%02d:<%s>' % (' '*26,toolOrder,tool)
                toolOrder += 1
            print
        return
