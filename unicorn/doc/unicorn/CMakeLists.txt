# -*- mode: CMAKE; explicit-buffer-name: # "CMakeLists.txt<unicorn/doc/unicorn>" -*-

               set ( htmlInstallDir  share/doc/coriolis2/en/html/unicorn )
               set ( latexInstallDir share/doc/coriolis2/en/latex/unicorn )
               set ( doxExtras       closed.png
                                     open.png
                                     jquery.js
                                     dynsections.js
                   )

 add_custom_target ( documentation ALL cd ${UNICORN_SOURCE_DIR}/doc/unicorn && ${DOXYGEN_EXECUTABLE} doxyfile )

           install ( DIRECTORY html/                DESTINATION ${htmlInstallDir} )
           install ( FILES     ${doxExtras}         DESTINATION ${htmlInstallDir} )

           install ( DIRECTORY latex/               DESTINATION ${latexInstallDir} )
           install ( FILES     socbook.cls          DESTINATION ${latexInstallDir} )
