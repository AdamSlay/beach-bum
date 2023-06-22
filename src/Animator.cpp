//
// Created by adam slay on 6/22/23.
//

#include "Animator.h"

/**
 * Player class contains an Animator as a member which takes care of all of the assets and animation
 * then you would just call player.render() which would call animator.animate(state=player_state) and render the current frame
 * on init(), the animator class acquires the assets and sets up the animation clip
 * it also keeps track of the current animation frame and the current animation clip
 * animation frames reset to 0 when the type of animation changes
 * this ensures that each animation starts from the beginning
 * The animator also needs to setup each animation clip
 * the clips are stored in a vector of SDL_Rects and correspond to the animation frames on the sprite sheet
 * the clips need to be split up into each animation type
 * for instance, the running animation has 8 frames, so the clips vector will have 8 elements
 * the frames for the running animation may be clips 0-7 on the sprite sheet
 * the jumping animation may be clips 8-11, etc.
 * these all need to be arranged into sequences that can be played from the beginning when you start that animation
 */

Animator::Animator() {

}

Animator::~Animator() {

}

void Animator::animate(SDL_Renderer* renderer, SDL_Rect* clip, int direction, float scale) {
    // render the texture
}
