# AI-Shooter
 
The game was created for a project in the course "Artificial Intelligence" at the academy.

The goal of the project is to demonstrate smart behavior of virtual characters. 
The game involves two teams, with each team having two soldiers and a combat supporter. 
The characters have different nature and tasks they can be perform. 
The characters make decisions depending on the situation they are in. 
The project uses the A * search algorithm to locate characters, storages and dig tunnels between rooms. 
During the game messages to the console are being printed, regarding the data of the characters and the tasks they have chosen to perform.

https://user-images.githubusercontent.com/68189545/154982554-1bd48e5c-1d20-4ce7-b92f-d7fe9fa3646f.mp4

[Click to view documentation in Hebrew](/Documentation.pdf)

### Game Map
- The size of the map is 100 by 100 squares.
- 10 rooms in random locations and sizes.
- Rooms can include all colours and types of squares (except black and crimson).
- Crimson squares symbolize the corridors that connect the rooms. 
  The connection is made using an A * search algorithm while finding the cheapest and shortest excavation route between the rooms. 
  Squares of this type can be occupied by characters.
- Black squares symbolize an exterior wall of the rooms and corridors of the map.
- Gray squares symbolize a free space that can be occupied by characters.
- Brown squares symbolize obstacles. In each room there are scattered obstacles in random quantity and locations depending on its size.
- Pink squares symbolize health storages. In each room are scattered two such storages in random locations.
- Copper-coloured squares symbolize ammunition storages. In each room are scattered two such storages in random locations.
- Dark blue squares symbolize soldiers of the blue group.
- A light blue square symbolizes the blue group's combat supporter.
- Dark green squares symbolize soldiers of the green group.
- A light green square symbolizes the green group’s combat supporter.

<img src="https://github.com/roi-c/AI-Shooter/blob/main/ImagesAndVideo/Game.png" alt="drawing" width="800"/>  

### Health and Ammunition
At the beginning of the game:
- Each character has 100 health points. The maximum amount of health points is 100 and the minimum amount is 0.
- A soldier has 16 ammunition units. The maximum amount of ammunition units is 16 and the minimum amount is 0.
- The combat supporter has a slick of 200 health points. The maximum amount of health points in Slick is 200 and the minimum amount is 0.
- The combat supporter has a slick of 20 ammunition units. The maximum amount of Slick ammunition units is 20 and the minimum amount is 0.
- Health storages contain 100 health points. When they are emptied, they turn into an obstacle.
- Ammunition storages contain 16 ammunition units. When they are emptied, they turn into an obstacle.

### End Game
The game ends in the following cases:
- Win / Loss - A team eliminates all the characters of the opposing team, or the opposing team has only a combat supporter left.
- Draw - Takes place in the following cases: 1. Both teams have been eliminated. 2. Only combat supporters remained 3. All ammunition in the game ran out.

### Weapons
#### Pistol
- Maximum firing range of 9 squares.
- The cost of firing is one unit of ammunition.
- Maximum damage 20. Minimal damage 10. The closer the shooting - the higher the damage.
- The conditions for firing are: 1. There is no active projectile. 2. Enough ammunition units. 3. The enemy within firing range. 
  4. The enemy is vulnerable (not behind an obstacle).
- To check if the enemy is vulnerable, an invisible dummy bullet is fired at the enemy.
#### Grenade
- Can only be used if the enemy is in the range of 4 to 5 squares.
- The cost of throwing a grenade is 4 units of ammunition.
- Consists of 8 pistol bullets.
- At the moment of the explosion, the bullets disperse in different directions.
- Each bullet has a permanent damage of 10.
- From the moment of the shot until the moment of the explosion, the grenade can pass over obstacles, characters, and storages.
- The conditions for throwing a grenade are: 1. There is no active grenade. 2. Enough ammunition units. 3. The enemy within range of the shot. 
  4. In the shot path there is no square of an outer wall. 5. One of the following two conditions is met:
    - The enemy is vulnerable (behind an obstacle). To test this an invisible dummy bullet is fired in the direction of the enemy.
    - A grenade has the potential to hit more than one enemy. To verify this, an invisible dummy grenade consists of 360 bullets is blown up around the target enemy.
#### General
- The bullets disappear if they have passed a range of 9 squares or hit an obstacle, an outer wall, a storage, an enemy, or a team member.
- The characters can not be harmed by friendlyfire.
- To differentiate between the projectiles of the teams, each team has a unique colour for its projectiles.

### Movement of Characters
- Characters can only occupy squares in grey or crimson color.
- Each square at a given moment can only occupy one character.
- How the characters move in the game map depends on the risk values of each square. At the beginning of each character's' turn (if minimal distance is met), an invisible dummy   grenade consists of 360 bullets explodes around each character from the opposing team. Each bullet that passes through a particular square increases its risk value. At the end   of the "turn" of each character the risk value in all updated squares returns to the default value, in preparation for the "turn" of the next character. In this way, each       character can consider the risk values relevant to itself at a given moment. The idea is to allow the characters to make wise decisions while considering the level of danger     at a given moment in the squares in the game map.
- The movement of the characters is based on an A * search algorithm which calculates the cheapest and shortest route, considering squares that cannot be crossed and the level     of risk of squares that can be crossed.
- The characters' positions and the movement of the projectiles are updated in the game map only after all the characters have finished making their "turn" decisions.

### The Characters
#### Soldier
- Can attack an enemy with a pistol or grenade.
- At the beginning of the game each fighter is randomly assigned critical thresholds in certain ranges for health points and ammunition units. The soldier goes into "survival"     mode when the amount of health points or the amount of ammunition units is equal to or smaller than the critical thresholds. In fact, every Soldier has a unique nature. There   can be more "brave" soldiers who take more risks and others who are more "conservative" who will prefer to "go for sure".
- **The soldier has 3 main modes:**
1. Grenade evasion - In case the soldier detects that an enemy grenade has been thrown at him, he will move to the safest slot in his area.
2. Survival - In case one of the soldiers’ values is equal to or less than the critical values, he will go into "survival" mode. In this mode the soldier will flee to the group    combat supporter to try and improve these values. When the soldier is within range of the combat supporter, he will go into "war" mode. If the group combat supporter is          eliminated, in a high probability the soldier will flee to a place that is considered safe and with a low probability will keep fighting.
3. War - Search And Destroy
   - Search - The soldier will perform a smart search of an enemy. He will first check if there are any enemies in his area. If there are no enemies, he will look for the            closest enemy in the whole map, otherwise he will look for the closest and most vulnerable enemy (not behind an obstacle) in the area. If there is no such enemy, he will        look for the closest enemy in the whole map.
   - Shooting - Within a certain range of probabilities the soldier will prefer the pistol over the grenade. That is, he will first check if shooting is possible. If shooting        is not possible, he will try to throw a grenade. If a grenade cannot be thrown, he will go in the direction of the enemy. Behavior will be similar when the soldier              prefers to use grenade over a pistol, meaning priority will be given to throwing the grenade.

<img src="https://github.com/roi-c/AI-Shooter/blob/main/ImagesAndVideo/SoldierBehavior.png" alt="drawing" width="800"/>  

#### Combat Supporter
- Can supply himself and his teammates with health points. In a single turn can supply up to 25 health points (inclusive) to only one member of the team.
- Can provide team members (soldiers) with ammunition units. In a single turn can supply up to 4 ammunition units for only one member of the team.
- The supply range for team members is up to 2 squares (including diagonally).
- Can collect ammunition and health points from storages.
- The collection range from the storages is one square (including diagonally).
- When a combat supporter goes to a storage or teammate, he will choose the safest square around them.
- Like a soldier, the combat supporter also has critical thresholds for the amount of health points.
- **The combat supporter has 3 main modes:**
1. Grenade evasion - In case the combat supporter detects that an enemy grenade has been thrown at him, he will move to the safest slot in his area 
2. Survival - In the case that the number of health points is less than or equal to the critical threshold value and the health points slick is empty, the combat supporter will    switch to "survival" mode. In this mode the combat supporter will look for the nearest health storage. If one exists, he will move towards it, otherwise he will go into         "support" mode.
3. Support - Group Status Monitoring - Includes 3 modes:
- Empty Slick - (Prerequisite is that there are storages). In this case the combat supporter will look for the most suitable storage. To select the most suitable storage           scores are calculated. When calculating the scores, the percentage of health slick inventory and the distance to the nearest health storage are considered compared to the       percentage of ammunition slick inventory and the distance to the nearest ammunition storage. The combat supporter will choose to go to the storage with the highest score.
- Providing health points or ammunition units - For each group member (including the combat supporter himself) a score is calculated based on his distance from the team combat     supporter and the condition of his health points and ammunition units. The team member with the highest score will be selected for supplying with health points or ammunition     units according to his main need at that moment. The score calculation is based on a table of ranges and scores for distance, health points and ammunition units.
- No need / ability to give assistance 
   - No need to give assistance - in case the health points and ammunition units of the whole team are full. The combat supporter will look for the team member closest to            him. If one exists, he will go to him, otherwise he will flee to a place that is considered safe.
   - There is no ability to give assistance - in case a team member needs a certain supply, but the combat supporter has no ability to give it because his slick is empty of          the same type of supply. In this case the combat supporter will look for the most suitable storage as described in "Empty Slick" above. In the absence of such a                  storage, the combat supporter will look for the team member closest to him. If one exists, he will go to him, otherwise he will flee to a place that is considered safe.               
<img src="https://github.com/roi-c/AI-Shooter/blob/main/ImagesAndVideo/SupportBehavior.png" alt="drawing" width="800"/>  

### Notes
Below notes are not the focus of the project, but it is good to consider:
- To enhance performance, it is better to activate Astar a little when target is far, and more often when target is near.
- While having only one character being able to occupy a square is more realistic, performance may suffer when corridor is blocked by characters.

 
