#  SolidsPack Version 8/14/07
#  SIMPLEXER: initQueue.tcl (version 4)
#=======================================================
# Move a queue file into the proper directory for
# execution and create queue status file
#
# Some trouble with vnmrwish when multiple file pointers
# are open, so avoid doing that
#=======================================================

#
# queueFile is the source file for the queue
#

proc popup msg {
    tk_messageBox -type ok -message $msg
}

set curExp [lindex $argv 0]
set queueFile [lindex $argv 1]

set idx [string last "/" $curExp]
incr idx -1

set queuePath [string range $curExp 0 $idx]
set queuePath [file join $queuePath "Simplexer/queues"]
set queueFile [file join $queuePath $queueFile]
set tempQueueFile [file join $curExp "queue"]

#
# if the queue file exists and is a queue
# then set status to 'running'
#

if { [file exists $queueFile] } {
   set fd_queue [open $queueFile]
   gets $fd_queue line
   if { [lindex $line 0] == "Simplex_Queue" } {
       set status "running"
    } else {
       set status "error"
    }
} else {
   set status "error"
}

#
# the queueStatusFile holds the status of the queue ('running' or 'error' here)
# and the name of the current method
#
gets $fd_queue line
close $fd_queue

set queueStatusFile [file join $curExp "queueState"]
set fd_status [open $queueStatusFile w]
puts $fd_status $status
if {$status == "error" } {
   close $fd_status
   exit
} else {
   puts $fd_status [lindex $line 0]
   close $fd_status
}

#
# tempQueueFile contains a list of the remaining
# files in the queue. Thus copy into an array
#

set fd_queue [open $queueFile]
set k 0
while { [ gets $fd_queue line ] != -1 } {
   set a($k) $line
   incr k 1
}
close $fd_queue



set fd_temp [open $tempQueueFile w]
for {set i 2} {$i < $k } {incr i 1} {
   puts $fd_temp $a($i)
}
close $fd_temp

exit


    
    
