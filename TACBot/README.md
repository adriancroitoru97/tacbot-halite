<h1> TACBot Halite Bot </h1>

<h2> The idea behind the Bot</h2>

We implemented 2 strategies for finding possible places for our bot to attack.

- First of all, we obtained all the squares that are in the immediate vicinity of
our bot's area and we sorted them so that we obtain the one with the biggest
strength. That becomes the target of our attack. So, we check if we have enough
strength to make the attack and if we don't, we stay still until we do.

- The second strategy concerns the internal squares, that we already have.
In order to not lose any strength when the internal squares become very
powerful, we decided to find out the closest direction that has an opponent
(north, south, east or west), and send some strength in that direction to our
own squares, so they can become more powerful and be able to attack more 
opponents.

- Also, it needs to be mentioned that if neither of these strategies work,
the bot makes a random move towards north or west.

<h3> TACBot Team: Cherciu Andrei, Croitoru Adrian-Valeriu, Diaconu Tudor-Gabriel </h3>
