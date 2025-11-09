#  SolidsPack Version 8/14/07
#  SIMPLEXER: initSimplex.tcl (version 4)
#==================================================================================
#
#   initSimplex.tcl
#
#   Presents parameter selection menus
#
#   User can load a stored set of parameters, save a set of parameters 
#   or create a set from menu selections
#
#   On exit initSimplex stores the set of selected parameters in the
#   local simplex_vars file. The file contains the parameter name, its
#   current value and the maximum and minimum values specified for the parameter
#   by vnmr(J).
#
#   When a file is read, only the parameter name is used to generate the
#   list of selected parameters. In this way we prevent interference with the
#   loading of a file in the simplexOptions.tcl program that follows
#   this one.
#
#=================================================================================

#=================================================================================
#
#   Change log
#
#   3/06, ceb:  remove method load/save. Move a more complete store save function
#   to simplexOptions.tcl
#
#=================================================================================

set ssColor gray75
set hdColor red
set textColor blue
tk_setPalette $ssColor

proc popup msg {
    tk_messageBox -type ok -message $msg
}

#=====================================================
#  Read Configuration File
#=====================================================

proc parseConfigFile fileName {
    global aqpArr name descr class nElements
    set nElements 0
    set fd [open $fileName r]
    while { [gets $fd line] != -1 } {
        if { [lindex $line 0] == "#" || [llength $line] == 0 } {
            continue
        }
        if { [llength $line] == 1 } {
            set CL [lindex $line 0]
        } else {
            set aqpArr($nElements,$class) $CL
            set aqpArr($nElements,$name) [lindex $line 0]
            set aqpArr($nElements,$descr) [lindex $line 1]
            incr nElements 1
        }              
    }
    close $fd
}
 
#=====================================================
#  functions to process curpar files
#=====================================================

#======================================================
# parIndex returns the index number of a variable
# in aqpArr or -1 if the variable is not in aqpArr
#======================================================

proc parIndex  par {
    global nElements
    global aqpArr
    set vName 0
    for {set i 0 } {$i < $nElements} {incr i 1} {
        if  { [string equal $aqpArr($i,$vName)  $par] } {
            return $i
        }
    }
    return -1
}

#===================================================
# Run through curpar file and extract info about our
# parameters
# Fill in aqpArr as we go
#
# This routine will capture :
#                             Value
#                             Type
#                             DGroup
#                             Max
#                             Min
#                             Step
#====================================================

#============================================================
# Changes:
#
#  2/13/06   test for validity of entries in the config file
#  2/13/06   acommodate expansion of protection word to 17 bits
#
#
#============================================================

proc parseCurpar curparFile {     
    set fd [open $curparFile  r]
    global aqpArr value min max step dgroup type active
    global parmin parmax parstep
    global nElements name
    global expDir
       
#number of parameter traits, number of entries in parameter array

    set nElem 10
    set nArr 0
    
    for {set i 0} {$i < $nElements } {incr i 1} {
        set found($i) 0
    }
    
    while { [gets $fd line] >= 0 } {   	 
        if {  [llength $line]  < 1} {
            continue
        }
#
# check whether this is a keeper
#
       
        set e0 [lindex $line 0];                     #name
        set idx  [parIndex  $e0 ]
        if  { $idx != -1 }  {
            set found($idx) 1
            set e1 [lindex $line 1];                 #subtype
            set e2 [lindex $line 2];                 #basic type
	    if { $e2 == 2 } {
	    	set strError $e2
		append strError " is a string! Cannot optimize"
		popup $strError
		close $fd
		return
	    }
            set e3 [lindex $line 3];                 #maxvalue
            set e4 [lindex $line 4];                 #minvalue
            set e5 [lindex $line 5];                 #stepsize
            set e6 [lindex $line 6];                 #Ggroup
            set e7 [lindex $line 7];                 #dgroup
            set e8 [lindex $line 8];                 #protection word
            set e9 [lindex $line 9];                 #active
            set e10 [lindex $line 10];               #int ptr              
            set aqpArr($idx,$type)   $e1
            set aqpArr($idx,$dgroup) $e7
            set aqpArr($idx,$active) $e9 
                       
#crack protect word

            set val 32768 
            for { set i 0 } {$i < 16 } {incr i 1} {
                set lProt($i) 0
            }
            for { set i 15 } {$i >= 0 } {incr i -1} {
                 if {$e8 >= $val} {
                 set lProt($i) 1
                 incr e8 -$val
            } 
            set val [expr $val/2]
        }

# check bit 13

        set k 13
        if { $lProt($k) == 1 } {
            set aqpArr($idx,$min)   [ lindex $parmin [ expr $e3 - 1] ]
            set aqpArr($idx,$max)   [ lindex $parmax [ expr $e4 - 1] ]
            set aqpArr($idx,$step)  [ lindex $parstep [ expr $e5 - 1] ]
        } else {
            set aqpArr($idx,$max) $e3
            set aqpArr($idx,$min) $e4
            set aqpArr($idx,$step) $e5
        }
        gets $fd line
        if { [lindex $line 0] !=  1} {
            set  strMsg $e0
            append strMsg "  Is an array!"
            popup $strMsg
	    close $fd
	    return
        } else {
            set aqpArr($idx,$value)  [lindex $line 1]
        }
        gets $fd line
    } else {
        if { [lindex $line 2] == 2 } {
            gets $fd line
	    set n [lindex $line 0]
            for {set i 0} {$i < $n} {incr i 1} {
            gets $fd line
        } 
	    } else {
	        gets $fd line
	        gets $fd line
	    }
        }
    }
    close $fd  

# All elements in aqpArr found in curpar?
   
   set configError 0
   set strError "Error in initSimplex: The following elements were not found in curpar: \n"
   for {set i 0} {$i < $nElements } {incr i 1 } {
        if {$found($i) == 0 } {
            set configError 1
            append strError $aqpArr($i,$name)
            append strError \n
        }
    }
    if {$configError == 1} {
        popup $strMsg
        abort $expDir
    }       
}
  
#
# If parmin/max and step are derivesd from arrays
#

proc parseConpar conparFile {

    global parmin
    global parmax
    global parstep

    set fd [open $conparFile r]

    while { [gets $fd line] >= 0 } {
        if { [lindex $line 0] == "parstep" } {
            gets $fd line
            for {set i 1} {$i < [llength $line] } {incr i 1} {
                lappend parstep  [lindex $line $i]
            }
            gets $fd line
            continue
        }

        if { [lindex $line 0] == "parmax" } {
            gets $fd line
            for {set i 1} {$i < [llength $line] } {incr i 1} {
                lappend parmax  [lindex $line $i]
            }
            gets $fd line
            continue
        }

        if { [lindex $line 0] == "parmin" } {
            gets $fd line
            for {set i 1} {$i < [llength $line] } {incr i 1} {
                lappend parmin  [lindex $line $i]
            }
            gets $fd line
            continue
        }
    }
    close $fd
}

proc setUnits { } {
    global aqpArr nElements dgroup type units name
    
    for {set i 0} {$i < $nElements } {incr i 1} {
        switch -exact -- $aqpArr($i,$type) {
           0 -
           1 -
           4 -
           7 -
           2 {set aqpArr($i,$units) " "}
           3 {set aqpArr($i,$units) "s"}
           5 {set aqpArr($i,$units) "Hz"}
           6 {set aqpArr($i,$units) "us"}
           default {set apqArr($i,$units) " "}
       }
   }
}

#===========================================================
#  Display Routines
#===========================================================

proc destroy_frames { } {
    global nElements
    global frameArr
    set n [array size frameArr]
    if { $n > 0 } {
        for  { set i 0 } {$i < $n } {incr i 1 }  {
             set k [expr 2*$i]
             pack forget $frameArr($k)
             destroy $frameArr($k)
             unset frameArr($k)
        }
    }
}

proc sel_all { } {
    global arrSel
    global nElements
    for { set i 0 } { $i < $nElements } {incr i 1 } {
        set arrSel($i) 1
    }

}

proc clear_all { } {
     global arrSel
     global nElements
     for { set i 0 } { $i < $nElements } {incr i 1 } {
         set arrSel($i) 0
    }
}

proc display iC {

    global aqpArr frameArr nElements ssColor textColor lClass eL arrSel
    global name descr value units min max step dgroup type class active
    
#============================
# destroy any previous frames
#============================

    destroy_frames

    set n [array size eL]
    if { $n > 0 } {unset eL}

#========================================
# prepare list of elements to show
# grey out and deselect inactive elements
#========================================

    if { $iC == "All"} {
        set iCount $nElements
        for { set i 0 } {$i < $nElements } {incr i 1 } {
            set eL($i) $i
        }
    }  else  { 
        if { $iC == "Selected" } {
            set iCount 0
            for { set i 0 } {$i < $nElements } {incr i 1 } {
                if { $arrSel($i) == 1} {
                    set eL($iCount) $i
                    incr iCount 1
                }
            }
        }  else  {
            set opt [lindex $lClass $iC]
            set iCount 0
            for { set i 0 } {$i < $nElements } {incr i 1 } {
                if { $aqpArr($i,$class) == $opt } {
                    set eL($iCount) $i
                    incr iCount 1
                }
            }
        }
    }

    for {set i 0 }  {$i  <  $iCount}  {incr i 2} {

#
#Create a frame to hold two checkbuttons
#

        frame .fa$i
        .fa$i configure -width 70
        set frameArr($i) .fa$i

        for {set nButton 0 } {$nButton < 2 } {incr nButton 1} {
            set    n [expr $i + $nButton]
            set    idx $eL($n)
            set    str1 $aqpArr($idx,$descr)
            append str1 " "
            append str1 $aqpArr($idx,$name)
            append str1 " "
            append str1  [format %3.2f $aqpArr($idx,$value)]
            append str1 " "
            append str1 $aqpArr($idx,$units)

            checkbutton .fa$i.$nButton -variable arrSel($idx)  -anchor w -width 40 -justify left  \
                                       -bg $ssColor  -text $str1
            if { $aqpArr($idx,$active) == 0 } {
                .fa$i.$nButton deselect
                .fa$i.$nButton configure -state disabled  
            }

            pack .fa$i.$nButton -side left  -anchor w -expand y

            if {$nButton == 0} {
                if { $i == [expr $iCount -1] } {
                    pack .fa$i -anchor w
                    break
                }
            }

            pack .fa$i -side top -fill x  -anchor w
        }
    }
}

#=========================================
# Generate / load Parameter File
#
#  3/06, ceb
#       Remove file protection
#
#=========================================

proc apply dirPath  {

    global aqpArr nElements arrSel strFileType eL name value min max units
    set strFileType "simplex_vars"
    set fileName [file join $dirPath $strFileType]
#    if { [file exists  $fileName] } {
#        set fileExists 1
#        set iResponse [tk_dialog  .dialogStore   "Parameter File Exists" \
#                                                 "Choose Option" \
#                                                 warning 0 \
#                                                 "Abort"  "Replace"]
#
#            switch -exact -- $iResponse {
#                0 {return}
#                1 {set openMode "w"
#                   file delete $fileName -force
#                  }
#           }
#    } else {
    set openMode "w"
#    }
   
    if {$openMode == "w"} {
        set fd [open $fileName "w"]
        puts $fd $strFileType
        for {set i 0} {$i < [array size arrSel] } {incr i 1} {
            if { $arrSel($i) == 1 } {
                puts -nonewline $fd $aqpArr($i,$name)
                puts -nonewline $fd " "
                puts -nonewline $fd $aqpArr($i,$value)
                puts -nonewline $fd " "
                puts -nonewline $fd $aqpArr($i,$min) 
                puts -nonewline $fd " "
                puts            $fd $aqpArr($i,$max) 
            }
        }
        close $fd
        exit          
    }
    return
}

proc abort dirPath {
    set fileName [file join $dirPath "simplex_vars"]
    set fd [open $fileName "w"]
    puts $fd "error"
    close $fd
    exit
}

proc load dir {
    global aqpArr nElements arrSel
    clear_all
   
    set methodFile [tk_getOpenFile -initialdir $dir]
    if [ catch {set fd [open $methodFile r] } ] {return}
    gets $fd line
    if { [lindex $line 0] != "simplex_vars" } {
        set strError "Error: "
        append strError $methodFile
        append strError " is not a valid Simplexer Method File! "
        popup $strError
        close $fd
        return
    }
   
    while { [gets $fd line] > 0 } {
        set vName [lindex $line 0]
        set arrSel([parIndex $vName]) 1
    }
    display "Selected"   
}

proc store dir {
    global arrSel eL aqpArr name value min max units
    set methodFile [tk_getSaveFile -initialdir $dir]
    if [ catch {set fd [open $methodFile w] } ] {return} 
    puts $fd "simplex_vars"
    
    for {set i 0} {$i < [array size arrSel] } {incr i 1} {
        if { $arrSel($i) == 1 } {
            puts -nonewline $fd $aqpArr($i,$name)
            puts -nonewline $fd " "
            puts -nonewline $fd $aqpArr($i,$value)
            puts -nonewline $fd " "
            puts -nonewline $fd $aqpArr($i,$min)
            puts -nonewline $fd " "
            puts            $fd $aqpArr($i,$max)      
        }
    }
    close $fd
    return          
}

#====================================
#
#   PROGRAM ENTRY POINT
#
#====================================
 
set name    0
set descr   1
set value   2
set units   3
set min     4
set max     5
set step    6
set dgroup  7
set type    8
set class   9
set active  10

set nElements 0

#====================================
#  Construct Paths
#====================================

set curExp [lindex $argv 0]
set pulseSequence [lindex $argv 1]
set expDir $curExp

set    idx1 [string last "/" $curExp]
set    userDir [string range $curExp 0 $idx1]
set    configFile [file join $userDir [file join "Simplexer" $pulseSequence] ]
append configFile ".s"
set    curparFile [file join $curExp "curpar"]
set    methodDir [file join $userDir "SimplexMethods"]
set    conparFile "/vnmr/conpar"

parseConfigFile $configFile
parseConpar $conparFile
parseCurpar $curparFile

setUnits

set iClass 0
set lClass [list $aqpArr($iClass,$class)]
set lClass [list ]
for { set i 1 } {$i < $nElements } { incr i 1 } {
    set strTem $aqpArr($i,$class)
    set k [lsearch  -exact $lClass $strTem]
    if { $k == -1 }  {
        lappend lClass $strTem
    }
}

#========================================
# Set Up Menus
#========================================

set wMain .
wm title $wMain  "Select Optimization Variables"
wm geometry . +50+150
$wMain configure -bg $ssColor

#create frame to hold a menu bar

frame .mb -bg $ssColor -borderwidth 1 -relief raised
pack .mb -fill x

#
#Top Level Menu Button
#

#menubutton .mb.f -text "File" -fg $textColor -bg $ssColor -menu .mb.f.m -relief raised
#pack .mb.f -side left

menubutton .mb.v -text "View" -fg $textColor -bg $ssColor -menu .mb.v.m -relief raised
pack .mb.v -side left

#
#Command Buttons
#

button .mb.b1 -text "Continue" -fg $textColor -bg $ssColor -command { apply $expDir  }
button .mb.b2 -text "Abort"  -fg $textColor -bg $ssColor -command { abort $expDir }
button .mb.b3 -text "Select All" -fg $textColor -bg $ssColor -command sel_all
button .mb.b4 -text "Clear" -fg $textColor -bg $ssColor -command clear_all

pack .mb.b3 .mb.b4 -side left
pack .mb.b1 .mb.b2 -side right

#
# Create File Menu
#
#menu .mb.f.m -tearoff false
#.mb.f.m add command -label "Load Method" -background $ssColor -command  {load $methodDir }
#.mb.f.m add command -label "Store Method" -background $ssColor -command {store $methodDir }

#
#Create View Menu
#
menu .mb.v.m -tearoff false

for { set i 0 } { $i < [llength $lClass] } { incr i 1 } {
    set cmd [list  display $i]
    .mb.v.m add radio -label [lindex $lClass $i] -background $ssColor -command  $cmd
}
.mb.v.m add radio -label All -background $ssColor -command  {display "All"}
.mb.v.m add radio -label Selected -background $ssColor -command  {display "Selected"}
clear_all
display "All"
