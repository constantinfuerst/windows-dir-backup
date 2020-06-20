## light windows directory backup utility
Automates the concurrent backup of a directory to another.

#### function
Given two full folder names, the utility will wait until a change occurs in the "directory" and then replicate this in the "destination".
 - adding or modifying an file will cause the file to be copied
 - renaming or moving the file will cause the file to be moved
 - deleting will cause one of two effects
   1. if bin enabled the file will be moved to a new destination (trash) with a user specified name (call it "bin" or "trash", whatever)
   2. if bin disabled the file will be deleted (on network drives this is not easily recoverable)

#### use case
When a backup task to a NAS needs to be automated with very low resource consumption and configuration.

#### future
 - Configuration options for excluding files or directories
 - installing as a service (with a client for configuration through cli)
 - multiple directories
 - verbose error handling

#### implementation
Uses the FindFirstChangeNotification windows api function to idle until a change is registered in a monitored directory.
After a specified delay (to allow for further file changes) the monitoring thread will read the NTFS change record (USN)
to obtain data on file changes during the sleep. These are evaluated and if found to have occured in the monitored directories
subtree, replicated in a specified destination directory.
