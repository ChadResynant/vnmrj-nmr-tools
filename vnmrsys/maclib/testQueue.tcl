#  SolidsPack Version 8/14/07
#  SIMPLEXER: testQueue.tcl (version 4)
#====================================================
#
# testQueue.tcl
#
#  Called by SIMPLEXER MAGICAL macro
#
#  Tests the status of the Simplexer queue.
#  and writes the state of the queue to
#  the current experiment. Queue status
#  includes the state of no queue.
#
#  Loads next method into the queue status
#  file 
#
#=====================================================
proc popup msg {
    tk_messageBox -type ok -message $msg
}

set curExp [lindex $argv 0]
set statusFile [file join $curExp "queueState"]
set queueFile [file join $curExp "queue"]

set fd_status [open $statusFile w]

if { [file exists $queueFile] } {
   set fd_queue [open $queueFile]
   if { [gets $fd_queue line] >= 0 } {
      set status "running"
      puts $fd_status $status
      puts $fd_status $line
      close $fd_status
   } else {
      set status "empty"
      puts $fd_status $status
      close $fd_status
      close $fd_queue
   }
} else {
     set status "no_queue_file"
     puts $fd_status $status
     close $fd_status
     exit
}

#
# update the queue
#
   set k 0
   while { [gets $fd_queue line] >= 0 } {
      set a($k) $line
      incr k 1
  }

close $fd_queue
set fd_queue [open $queueFile w]
for { set i 0 } { $i < $k } { incr i 1 } {
   puts $fd_queue $a($i)
}

close $fd_queue
exit  
   
