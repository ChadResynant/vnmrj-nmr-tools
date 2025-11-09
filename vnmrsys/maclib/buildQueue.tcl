#  SolidsPack Version 8/14/07
#  SIMPLEXER: buildQueue.tcl (version 4)
#=================================================
# Assemble a queue from methods Directory Entries
# and store the result in the queues directory
#
# Called by the buildQueue MAGICAL macro
#==================================================


global ssColor
set ssColor gray75
set hdColor red
set textColor blue
tk_setPalette $ssColor

proc popup msg {
    tk_messageBox -type ok -message $msg
}

proc loadDir  { } {
    
   global queueDir  methodDir dirList  
   list delete 0 end
   
   foreach i [lsort [glob -nocomplain -directory $methodDir *]] {
      if { [file isdirectory $i] } {
          .leftFrame.lb insert end [file tail $i]
      }
   }
}


proc getDir { } {
   global methodDir
   set dir [tk_chooseDirectory -initialdir $methodDir -title "Chose Directory"]
   if {$dir eq ""} {
      return
    } else {
       set methodDir $dir
       .leftFrame.lb delete 0 end
       loadDir
    }
}

proc ok {} {
global methodDir queueDir

   set ofile [tk_getSaveFile -initialdir $queueDir -title "Store Queue"]
   if {$ofile eq ""} {
      popup "No file selected"
      return
    } 
    
    
    set fd [ open $ofile w ]
    puts $fd "Simplex_Queue"
    set n [.rightFrame.lb  size]
    for {set i 0 } {$i < $n } {incr i 1 } {
       puts $fd [.rightFrame.lb  get $i]
    } 
    close $fd
}

  set wMain .
  wm title  $wMain "Assemble a Queue"
  $wMain configure -bg $ssColor

  frame .buttonFrame
  pack .buttonFrame -side top -fill x



  button .buttonFrame.chooseDir -text  "Change Method Dir"  -bg $ssColor  -command getDir
  button .buttonFrame.oK -text "Save Queue"   -bg $ssColor  -command ok
  button .buttonFrame.babort  -text  "Exit"    -bg $ssColor -command exit

  pack .buttonFrame.chooseDir .buttonFrame.babort -side left -anchor w
  pack .buttonFrame.oK  -side right -anchor e

  

   

  frame .leftFrame
  frame .rightFrame

  label .leftFrame.l -text "Methods"
  label .rightFrame.l -text "Queue"

  listbox  .leftFrame.lb -yscroll ".leftFrame.scroll set" \
                         -width 20 -height 20 -setgrid 1
                                             
  scrollbar .leftFrame.scroll  -command  ".leftFrame.lb yview"


  pack .leftFrame -side left
  pack .leftFrame.l -side top
  pack  .leftFrame.lb .leftFrame.scroll  -side left -expand yes -fill y


 listbox  .rightFrame.lb -yscroll ".rightFrame.scroll set" \
                                             -width 20 -height 20 -setgrid 1 \
                                             -selectmode extended \
                                             -activestyle none
                                             
 scrollbar .rightFrame.scroll  -command  ".rightFrame.lb yview"


pack .rightFrame -side right
pack .rightFrame.l -side top
pack  .rightFrame.lb .rightFrame.scroll  -side left -expand yes -fill y


#
# Give the right list box keyboard focus
#

focus  .rightFrame.lb


bind .leftFrame.lb <Double-1>  {
   set strMsg [selection get] 
   .rightFrame.lb  insert end $strMsg
}

bind .rightFrame.lb <Double-2>  {
   set beg [lindex [.rightFrame.lb curselection] 0]
   set fin [lindex [.rightFrame.lb curselection] [expr [llength [.rightFrame.lb curselection]] -1 ]]
   .rightFrame.lb delete $beg $fin
}

bind .rightFrame.lb  <KeyPress-BackSpace>  {
  .rightFrame.lb delete [.rightFrame.lb curselection] 
}

bind .rightFrame.lb  <Control-KeyPress-c>  {
  global copyList
  set beg [lindex [.rightFrame.lb curselection] 0]
  set fin [lindex [.rightFrame.lb curselection] [expr [llength [.rightFrame.lb curselection]] -1 ]]
  set copyList  [.rightFrame.lb  get  $beg $fin]
}

bind .rightFrame.lb  <Control-KeyPress-x>  {
  set beg [lindex [.rightFrame.lb curselection] 0]
  set fin [lindex [.rightFrame.lb curselection] [expr [llength [.rightFrame.lb curselection]] -1 ]]
  .rightFrame.lb delete $beg $fin
}

bind .rightFrame.lb  <Control-KeyPress-v>  {
    global copyList
    set fin [lindex [.rightFrame.lb curselection] [ llength [.rightFrame.lb curselection]] ]
    if {$fin == [.rightFrame.lb size] } {
       for {set i 0 } {$i < [llength $copyList] } {incr i 1 } {
          .rightFrame.lb insert end  [lindex $copyList $i]
       }
    } else {
       set ll [.rightFrame.lb curselection]
       set fin [lindex $ll]
       incr fin 1
       for {set i 0 } {$i < [llength $copyList] } {incr i 1 } {
          .rightFrame.lb insert $fin  [lindex $copyList $i]
          incr fin 1
       }
        
     }   
}

set queueDir [lindex $argv 0]
set methodDir [lindex $argv 1 ]

loadDir 




