# A set of probabilities p(z|x), where z is the current measurements, and x is the set of possible different locations.

# These probabilities must be in the range (0,1), but can sum to any value over the set. 

std_msgs/Header header   # Seems to normally be in messages

float32[] probabilities    # An array for the set of probabilites. 
                           # These will be in a particular order, following Luan's state definition for now:
                           # 0 - left-branch
                           # 1 - straight-centre
                           # 2 - straight-sided
                           # 3 - sided
                           # 4 - left corner
                           # 5 - crash left
                           # 6 - crash right
                           # 7 - cross
                           # 8 - T-junction
                           # 9 - right corner
                           # 10 - right branch
                           # 11 - dead end
                           # 12 - flip risk
                           # 13 - open space
                           # It would be great to be able to communicate this mapping in the message too; I'm not sure how best to do this.

