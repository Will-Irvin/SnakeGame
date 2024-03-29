# SnakeGame
This is my own version of the classic Snake arcade game with additional customization for the player. Since many recent games have added more advanced customization options for players, I decided to do something similar with a less complex game on my own. 

After completing an SDL tutorial I wanted to work on a project where I could apply the tool I had learned. I thought of Snake as a simple first project to develop in SDL.

## Features
This project uses core SDL, the SDL_ttf extension, and the SDL_image extension. TTF fonts are used to display the text on the intialization screen and the game over screen. The initialization screen tells the player how to play this version of the game and allows them to customize certain aspects of the game including: how long it takes the snake to move, whether the snake gets faster after eating an apple, how large the grid that the snake moves around in is, and how many apples appear at a single time. The player can also press a key to have the screen be automatically resized to have the specified grid appear as square tiles. Finally, SDL_image can be used to display the player's own image as the head of the snake.

The game is rendered using SDL geometry and each frame is rendered in increments that the player specifies during initialization. 

Finally, the game over screen appears when the player loses and uses TTF to display the high score and the score from the last round. After leaving the game over screen, the player can change attributes of the game and start again. 

## How to Use
This repository contains each of the source code files and a Makefile to compile them into the final "Main" executable. It does require SDL version 2 to be installed. To display their own image player only needs to upload an image named "snake_head" in png or jpg format to the images folder. If no image is loaded, a green rectangle will just be used for the head. Since only one image will be loaded the program will try png first before jpg. If png succeeds a jpg image will not be loaded. This repository includes two images as an example, but anyone could use any image as long as they name it snake_head. 


Note: this code was originally written and run using Windows Subsystem for Linux.
