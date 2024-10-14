size of message  -> fixed    -> 4
type of message  -> fixed    -> 1
from(port)	 -> fixed    -> 2
to(port)         -> fixed    -> 2
to systemId      -> encoded  -> U
data             -> will vary but not to be encoded -> BUFLEN - (4 + U + 5)

size of message <= BUFLEN



