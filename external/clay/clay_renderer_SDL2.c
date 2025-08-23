#include "clay.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <string.h>  // Added for memcpy function

// Define SDL2_Font struct in C-compatible way
typedef struct {
    uint32_t id;
    TTF_Font* font;
} SDL2_Font;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define CLAY_COLOR_TO_SDL_COLOR_ARGS(color) color.r, color.g, color.b, color.a

#ifdef __cplusplus
extern "C" {
#endif

// Function declaration for proper C++ compatibility - NOT static!
Clay_Dimensions SDL2_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);

Clay_Dimensions SDL2_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData)
{
    SDL2_Font *fonts = (SDL2_Font*)userData;
    if (!fonts || !config) {
        fprintf(stderr, "Error: Invalid fonts or config pointer\n");
        Clay_Dimensions dimensions;
        memset(&dimensions, 0, sizeof(dimensions));
        return dimensions;
    }

    TTF_Font *font = fonts[config->fontId].font;
    if (!font) {
        fprintf(stderr, "Error: Invalid font for fontId %d\n", config->fontId);
        Clay_Dimensions dimensions;
        memset(&dimensions, 0, sizeof(dimensions));
        return dimensions;
    }
    
    TTF_SetFontSize(font, config->fontSize);
    char *chars = (char *)calloc(text.length + 1, 1);
    if (!chars) {
        fprintf(stderr, "Error: Memory allocation failed for text measure\n");
        Clay_Dimensions dimensions;
        memset(&dimensions, 0, sizeof(dimensions));
        return dimensions;
    }
    
    memcpy(chars, text.chars, text.length);
    int width = 0;
    int height = 0;
    if (TTF_SizeUTF8(font, chars, &width, &height) < 0) {
        fprintf(stderr, "Error: could not measure text: %s\n", TTF_GetError());
        free(chars);
        Clay_Dimensions dimensions;
        memset(&dimensions, 0, sizeof(dimensions));
        return dimensions;
    }
    free(chars);
    
    Clay_Dimensions dimensions;
    dimensions.width = (float)width;
    dimensions.height = (float)height;
    return dimensions;
}

/* Global for convenience. Even in 4K this is enough for smooth curves (low radius or rect size coupled with
 * no AA or low resolution might make it appear as jagged curves) */
static int NUM_CIRCLE_SEGMENTS = 16;

//all rendering is performed by a single SDL call, avoiding multiple RenderRect + plumbing choice for circles.
static void SDL_RenderFillRoundedRect(SDL_Renderer* renderer, const SDL_FRect rect, const float cornerRadius, const Clay_Color _color) {
    SDL_Color color;
    color.r = (Uint8)_color.r;
    color.g = (Uint8)_color.g;
    color.b = (Uint8)_color.b;
    color.a = (Uint8)_color.a;

    int indexCount = 0, vertexCount = 0;

    const float maxRadius = SDL_min(rect.w, rect.h) / 2.0f;
    const float clampedRadius = SDL_min(cornerRadius, maxRadius);

    const int numCircleSegments = SDL_max(NUM_CIRCLE_SEGMENTS, (int)clampedRadius * 0.5f);

    SDL_Vertex vertices[512];
    int indices[512];

    //define center rectangle
    SDL_Vertex v0;
    v0.position.x = rect.x + clampedRadius;
    v0.position.y = rect.y + clampedRadius;
    v0.color = color;
    v0.tex_coord.x = 0;
    v0.tex_coord.y = 0;
    vertices[vertexCount++] = v0; //0 center TL

    SDL_Vertex v1;
    v1.position.x = rect.x + rect.w - clampedRadius;
    v1.position.y = rect.y + clampedRadius;
    v1.color = color;
    v1.tex_coord.x = 1;
    v1.tex_coord.y = 0;
    vertices[vertexCount++] = v1; //1 center TR

    SDL_Vertex v2;
    v2.position.x = rect.x + rect.w - clampedRadius;
    v2.position.y = rect.y + rect.h - clampedRadius;
    v2.color = color;
    v2.tex_coord.x = 1;
    v2.tex_coord.y = 1;
    vertices[vertexCount++] = v2; //2 center BR

    SDL_Vertex v3;
    v3.position.x = rect.x + clampedRadius;
    v3.position.y = rect.y + rect.h - clampedRadius;
    v3.color = color;
    v3.tex_coord.x = 0;
    v3.tex_coord.y = 1;
    vertices[vertexCount++] = v3; //3 center BL

    indices[indexCount++] = 0;
    indices[indexCount++] = 1;
    indices[indexCount++] = 3;
    indices[indexCount++] = 1;
    indices[indexCount++] = 2;
    indices[indexCount++] = 3;

    //define rounded corners as triangle fans
    const float step = (M_PI / 2) / numCircleSegments;
    for (int i = 0; i < numCircleSegments; i++) {
        const float angle1 = (float)i * step;
        const float angle2 = ((float)i + 1.0f) * step;

        for (int j = 0; j < 4; j++) {  // Iterate over four corners
            float cx, cy, signX, signY;

            switch (j) {
            case 0: cx = rect.x + clampedRadius; cy = rect.y + clampedRadius; signX = -1; signY = -1; break; // Top-left
            case 1: cx = rect.x + rect.w - clampedRadius; cy = rect.y + clampedRadius; signX = 1; signY = -1; break; // Top-right
            case 2: cx = rect.x + rect.w - clampedRadius; cy = rect.y + rect.h - clampedRadius; signX = 1; signY = 1; break; // Bottom-right
            case 3: cx = rect.x + clampedRadius; cy = rect.y + rect.h - clampedRadius; signX = -1; signY = 1; break; // Bottom-left
            default: return;
            }

            SDL_Vertex vCorner1;
            vCorner1.position.x = cx + SDL_cosf(angle1) * clampedRadius * signX;
            vCorner1.position.y = cy + SDL_sinf(angle1) * clampedRadius * signY;
            vCorner1.color = color;
            vCorner1.tex_coord.x = 0;
            vCorner1.tex_coord.y = 0;
            vertices[vertexCount++] = vCorner1;

            SDL_Vertex vCorner2;
            vCorner2.position.x = cx + SDL_cosf(angle2) * clampedRadius * signX;
            vCorner2.position.y = cy + SDL_sinf(angle2) * clampedRadius * signY;
            vCorner2.color = color;
            vCorner2.tex_coord.x = 0;
            vCorner2.tex_coord.y = 0;
            vertices[vertexCount++] = vCorner2;

            indices[indexCount++] = j;  // Connect to corresponding central rectangle vertex
            indices[indexCount++] = vertexCount - 2;
            indices[indexCount++] = vertexCount - 1;
        }
    }

    //Define edge rectangles
    // Top edge
    SDL_Vertex vTopL;
    vTopL.position.x = rect.x + clampedRadius;
    vTopL.position.y = rect.y;
    vTopL.color = color;
    vTopL.tex_coord.x = 0;
    vTopL.tex_coord.y = 0;
    vertices[vertexCount++] = vTopL; //TL

    SDL_Vertex vTopR;
    vTopR.position.x = rect.x + rect.w - clampedRadius;
    vTopR.position.y = rect.y;
    vTopR.color = color;
    vTopR.tex_coord.x = 1;
    vTopR.tex_coord.y = 0;
    vertices[vertexCount++] = vTopR; //TR

    indices[indexCount++] = 0;
    indices[indexCount++] = vertexCount - 2; //TL
    indices[indexCount++] = vertexCount - 1; //TR
    indices[indexCount++] = 1;
    indices[indexCount++] = 0;
    indices[indexCount++] = vertexCount - 1; //TR
    
    // Right edge
    SDL_Vertex vRightT;
    vRightT.position.x = rect.x + rect.w;
    vRightT.position.y = rect.y + clampedRadius;
    vRightT.color = color;
    vRightT.tex_coord.x = 1;
    vRightT.tex_coord.y = 0;
    vertices[vertexCount++] = vRightT; //RT

    SDL_Vertex vRightB;
    vRightB.position.x = rect.x + rect.w;
    vRightB.position.y = rect.y + rect.h - clampedRadius;
    vRightB.color = color;
    vRightB.tex_coord.x = 1;
    vRightB.tex_coord.y = 1;
    vertices[vertexCount++] = vRightB; //RB

    indices[indexCount++] = 1;
    indices[indexCount++] = vertexCount - 2; //RT
    indices[indexCount++] = vertexCount - 1; //RB
    indices[indexCount++] = 2;
    indices[indexCount++] = 1;
    indices[indexCount++] = vertexCount - 1; //RB
    
    // Bottom edge
    SDL_Vertex vBottomR;
    vBottomR.position.x = rect.x + rect.w - clampedRadius;
    vBottomR.position.y = rect.y + rect.h;
    vBottomR.color = color;
    vBottomR.tex_coord.x = 1;
    vBottomR.tex_coord.y = 1;
    vertices[vertexCount++] = vBottomR; //BR

    SDL_Vertex vBottomL;
    vBottomL.position.x = rect.x + clampedRadius;
    vBottomL.position.y = rect.y + rect.h;
    vBottomL.color = color;
    vBottomL.tex_coord.x = 0;
    vBottomL.tex_coord.y = 1;
    vertices[vertexCount++] = vBottomL; //BL

    indices[indexCount++] = 2;
    indices[indexCount++] = vertexCount - 2; //BR
    indices[indexCount++] = vertexCount - 1; //BL
    indices[indexCount++] = 3;
    indices[indexCount++] = 2;
    indices[indexCount++] = vertexCount - 1; //BL
    
    // Left edge
    SDL_Vertex vLeftB;
    vLeftB.position.x = rect.x;
    vLeftB.position.y = rect.y + rect.h - clampedRadius;
    vLeftB.color = color;
    vLeftB.tex_coord.x = 0;
    vLeftB.tex_coord.y = 1;
    vertices[vertexCount++] = vLeftB; //LB

    SDL_Vertex vLeftT;
    vLeftT.position.x = rect.x;
    vLeftT.position.y = rect.y + clampedRadius;
    vLeftT.color = color;
    vLeftT.tex_coord.x = 0;
    vLeftT.tex_coord.y = 0;
    vertices[vertexCount++] = vLeftT; //LT

    indices[indexCount++] = 3;
    indices[indexCount++] = vertexCount - 2; //LB
    indices[indexCount++] = vertexCount - 1; //LT
    indices[indexCount++] = 0;
    indices[indexCount++] = 3;
    indices[indexCount++] = vertexCount - 1; //LT

    // Render everything
    SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);
}

//all rendering is performed by a single SDL call, using two sets of arcing triangles, inner and outer, that fit together; along with two triangles to fill the end gaps.
static void SDL_RenderCornerBorder(SDL_Renderer *renderer, Clay_BoundingBox* boundingBox, Clay_BorderRenderData* config, int cornerIndex, Clay_Color _color){
    /////////////////////////////////
    //The arc is constructed of outer triangles and inner triangles
    //First three vertices are first outer triangle's vertices
    //Each two vertices after that are the inner-middle and second-outer vertex of 
    //each outer triangle after the first, because there first-outer vertex is equal to the
    //second-outer vertex of the previous triangle. Indices set accordingly.
    //The final two vertices are the missing vertices for the first and last inner triangles
    //Everything is in clockwise order (CW).
    /////////////////////////////////
    SDL_Color color;
    color.r = (Uint8)_color.r;
    color.g = (Uint8)_color.g;
    color.b = (Uint8)_color.b;
    color.a = (Uint8)_color.a;

    float centerX, centerY, outerRadius, startAngle, borderWidth;
    const float maxRadius = SDL_min(boundingBox->width, boundingBox->height) / 2.0f;
    
    SDL_Vertex vertices[512];
    int indices[512];
    int indexCount = 0, vertexCount = 0;

    switch (cornerIndex) {
        case(0):
            startAngle = M_PI; 
            outerRadius = SDL_min(config->cornerRadius.topLeft, maxRadius);
            centerX = boundingBox->x + outerRadius; 
            centerY = boundingBox->y + outerRadius; 
            borderWidth = config->width.top;
        break;
        case(1):
            startAngle = 3*M_PI/2;
            outerRadius = SDL_min(config->cornerRadius.topRight, maxRadius);
            centerX = boundingBox->x + boundingBox->width - outerRadius; 
            centerY = boundingBox->y + outerRadius; 
            borderWidth = config->width.top;
            break;
        case(2):
            startAngle = 0;
            outerRadius = SDL_min(config->cornerRadius.bottomRight, maxRadius);
            centerX = boundingBox->x + boundingBox->width - outerRadius; 
            centerY = boundingBox->y + boundingBox->height - outerRadius; 
            borderWidth = config->width.bottom;
            break;
        case(3):
            startAngle = M_PI/2;
            outerRadius = SDL_min(config->cornerRadius.bottomLeft, maxRadius);
            centerX = boundingBox->x + outerRadius; 
            centerY = boundingBox->y + boundingBox->height - outerRadius; 
            borderWidth = config->width.bottom;
            break;
        default: return;
    }
    
    const float innerRadius = outerRadius - borderWidth;
    const int minNumOuterTriangles = NUM_CIRCLE_SEGMENTS;
    const int numOuterTriangles = SDL_max(minNumOuterTriangles, ceilf(outerRadius * 0.5f));
    const float angleStep = M_PI / (2.0*(float)numOuterTriangles);

    //outer triangles, in CW order
    for (int i = 0; i < numOuterTriangles; i++) { 
        float angle1 =  startAngle + i*angleStep; //first-outer vertex angle
        float angle2 =  startAngle + ((float)i + 0.5) * angleStep; //inner-middle vertex angle
        float angle3 =  startAngle + (i+1)*angleStep; // second-outer vertex angle

        if( i == 0){ //first outer triangle
            SDL_Vertex vFirst;
            vFirst.position.x = centerX + SDL_cosf(angle1) * outerRadius;
            vFirst.position.y = centerY + SDL_sinf(angle1) * outerRadius;
            vFirst.color = color;
            vFirst.tex_coord.x = 0;
            vFirst.tex_coord.y = 0;
            vertices[vertexCount++] = vFirst; //vertex index = 0
        }
        indices[indexCount++] = vertexCount - 1; //will be second-outer vertex of last outer triangle if not first outer triangle.

        SDL_Vertex vMiddle;
        if (innerRadius > 0) {
            vMiddle.position.x = centerX + SDL_cosf(angle2) * (innerRadius);
            vMiddle.position.y = centerY + SDL_sinf(angle2) * (innerRadius);
        } else {
            vMiddle.position.x = centerX;
            vMiddle.position.y = centerY;
        }
        vMiddle.color = color;
        vMiddle.tex_coord.x = 0;
        vMiddle.tex_coord.y = 0;
        vertices[vertexCount++] = vMiddle;
        indices[indexCount++] = vertexCount - 1;

        SDL_Vertex vOuter;
        vOuter.position.x = centerX + SDL_cosf(angle3) * outerRadius;
        vOuter.position.y = centerY + SDL_sinf(angle3) * outerRadius;
        vOuter.color = color;
        vOuter.tex_coord.x = 0;
        vOuter.tex_coord.y = 0;
        vertices[vertexCount++] = vOuter;
        indices[indexCount++] = vertexCount - 1;
    }

    if(innerRadius > 0){
        // inner triangles in CW order (except the first and last)
        for (int i = 0; i < numOuterTriangles - 1; i++){ //skip the last outer triangle
            if(i==0){ //first outer triangle -> second inner triangle
                indices[indexCount++] = 1; //inner-middle vertex of first outer triangle
                indices[indexCount++] = 2; //second-outer vertex of first outer triangle
                indices[indexCount++] = 3; //innder-middle vertex of second-outer triangle
            }else{
                int baseIndex = 3; //skip first outer triangle
                indices[indexCount++] = baseIndex + (i-1)*2; // inner-middle vertex of current outer triangle
                indices[indexCount++] = baseIndex + (i-1)*2 + 1; // second-outer vertex of current outer triangle
                indices[indexCount++] = baseIndex + (i-1)*2 + 2; // inner-middle vertex of next outer triangle
            }
        }

        float endAngle = startAngle + M_PI/2.0;

        //last inner triangle
        indices[indexCount++] = vertexCount - 2; //inner-middle vertex of last outer triangle
        indices[indexCount++] = vertexCount - 1; //second-outer vertex of last outer triangle
        SDL_Vertex vEndInner;
        vEndInner.position.x = centerX + SDL_cosf(endAngle) * innerRadius;
        vEndInner.position.y = centerY + SDL_sinf(endAngle) * innerRadius;
        vEndInner.color = color;
        vEndInner.tex_coord.x = 0;
        vEndInner.tex_coord.y = 0;
        vertices[vertexCount++] = vEndInner; //missing vertex
        indices[indexCount++] = vertexCount - 1; 
        
        // //first inner triangle
        indices[indexCount++] = 0; //first-outer vertex of first outer triangle
        indices[indexCount++] = 1; //inner-middle vertex of first outer triangle
        SDL_Vertex vStartInner;
        vStartInner.position.x = centerX + SDL_cosf(startAngle) * innerRadius;
        vStartInner.position.y = centerY + SDL_sinf(startAngle) * innerRadius;
        vStartInner.color = color;
        vStartInner.tex_coord.x = 0;
        vStartInner.tex_coord.y = 0;
        vertices[vertexCount++] = vStartInner; //missing vertex
        indices[indexCount++] = vertexCount - 1; 
    }

    SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);
}

SDL_Rect currentClippingRectangle;

void Clay_SDL2_Render(SDL_Renderer *renderer, Clay_RenderCommandArray renderCommands, SDL2_Font *fonts)
{
    if (!renderer) {
        fprintf(stderr, "Error: Null renderer provided to Clay_SDL2_Render\n");
        return;
    }

    if (!fonts) {
        fprintf(stderr, "Error: Null fonts array provided to Clay_SDL2_Render\n");
        return;
    }

    for (uint32_t i = 0; i < renderCommands.length; i++)
    {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, i);
        
        // Skip invalid render commands
        if (!renderCommand) {
            continue;
        }
        
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
        
    // Use renderCommand->commandType directly so valid commands are matched
    switch (renderCommand->commandType)
        {
            case CLAY_RENDER_COMMAND_TYPE_NONE:
                // Skip null commands
                continue;

            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                Clay_Color color = config->backgroundColor;
                SDL_SetRenderDrawColor(renderer, CLAY_COLOR_TO_SDL_COLOR_ARGS(color));
                
                SDL_FRect rect;
                rect.x = boundingBox.x;
                rect.y = boundingBox.y;
                rect.w = boundingBox.width;
                rect.h = boundingBox.height;
                
                if (config->cornerRadius.topLeft > 0) {
                    SDL_RenderFillRoundedRect(renderer, rect, config->cornerRadius.topLeft, color);
                }
                else {
                    SDL_RenderFillRectF(renderer, &rect);
                }
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *config = &renderCommand->renderData.text;
                
                // Skip if text has no content
                if (config->stringContents.length == 0 || !config->stringContents.chars) {
                    continue;
                }
                
                char *cloned = (char *)calloc(config->stringContents.length + 1, 1);
                if (!cloned) {
                    fprintf(stderr, "Error: Memory allocation failed for text render\n");
                    continue;
                }
                
                memcpy(cloned, config->stringContents.chars, config->stringContents.length);
                
                // Ensure font ID is valid
                if (config->fontId >= 10) { // Assuming max 10 fonts for safety
                    fprintf(stderr, "Error: Invalid font ID: %d\n", config->fontId);
                    free(cloned);
                    continue;
                }
                
                TTF_Font* font = fonts[config->fontId].font;
                if (!font) {
                    fprintf(stderr, "Error: Null font for ID: %d\n", config->fontId);
                    free(cloned);
                    continue;
                }
                
                // Enable dynamic font resizing and hinting for crisp text
                TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
                TTF_SetFontSize(font, config->fontSize);
                
                // Create SDL_Color with proper initialization
                SDL_Color textColor;
                textColor.r = (Uint8)config->textColor.r;
                textColor.g = (Uint8)config->textColor.g;
                textColor.b = (Uint8)config->textColor.b;
                textColor.a = (Uint8)config->textColor.a;
                
                SDL_Surface *surface = TTF_RenderUTF8_Blended(font, cloned, textColor);
                if (!surface) {
                    fprintf(stderr, "Error: Failed to render text: %s\n", TTF_GetError());
                    free(cloned);
                    continue;
                }
                
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                if (!texture) {
                    fprintf(stderr, "Error: Failed to create texture from text surface: %s\n", SDL_GetError());
                    SDL_FreeSurface(surface);
                    free(cloned);
                    continue;
                }

                // Create SDL_Rect with proper initialization
                SDL_Rect destination;
                destination.x = (int)boundingBox.x;
                destination.y = (int)boundingBox.y;
                destination.w = (int)boundingBox.width;
                destination.h = (int)boundingBox.height;
                
                SDL_RenderCopy(renderer, texture, NULL, &destination);

                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
                free(cloned);
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                // Initialize clipping rectangle
                currentClippingRectangle.x = (int)boundingBox.x;
                currentClippingRectangle.y = (int)boundingBox.y;
                currentClippingRectangle.w = (int)boundingBox.width;
                currentClippingRectangle.h = (int)boundingBox.height;
                
                SDL_RenderSetClipRect(renderer, &currentClippingRectangle);
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                SDL_RenderSetClipRect(renderer, NULL);
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                Clay_ImageRenderData *config = &renderCommand->renderData.image;

                if (!config->imageData) {
                    continue;
                }

                // Cast void* to SDL_Surface* - assuming imageData is actually an SDL_Surface*
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, (SDL_Surface*)config->imageData);
                if (!texture) {
                    fprintf(stderr, "Error: Failed to create texture from image surface: %s\n", SDL_GetError());
                    continue;
                }

                // Create SDL_Rect with proper initialization
                SDL_Rect destination;
                destination.x = (int)boundingBox.x;
                destination.y = (int)boundingBox.y;
                destination.w = (int)boundingBox.width;
                destination.h = (int)boundingBox.height;

                SDL_RenderCopy(renderer, texture, NULL, &destination);

                SDL_DestroyTexture(texture);
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                Clay_BorderRenderData *config = &renderCommand->renderData.border;
                SDL_SetRenderDrawColor(renderer, CLAY_COLOR_TO_SDL_COLOR_ARGS(config->color));

                if((boundingBox.width <= 0) || (boundingBox.height <= 0)) {
                    continue;
                }

                const float maxRadius = SDL_min(boundingBox.width, boundingBox.height) / 2.0f;

                if (config->width.left > 0) {
                    const float clampedRadiusTop = SDL_min((float)config->cornerRadius.topLeft, maxRadius);
                    const float clampedRadiusBottom = SDL_min((float)config->cornerRadius.bottomLeft, maxRadius);
                    
                    SDL_FRect rect;
                    rect.x = boundingBox.x;
                    rect.y = boundingBox.y + clampedRadiusTop;
                    rect.w = (float)config->width.left;
                    rect.h = (float)boundingBox.height - clampedRadiusTop - clampedRadiusBottom;
                    
                    SDL_RenderFillRectF(renderer, &rect);
                }

                if (config->width.right > 0) {
                    const float clampedRadiusTop = SDL_min((float)config->cornerRadius.topRight, maxRadius);
                    const float clampedRadiusBottom = SDL_min((float)config->cornerRadius.bottomRight, maxRadius);
                    
                    SDL_FRect rect;
                    rect.x = boundingBox.x + boundingBox.width - config->width.right;
                    rect.y = boundingBox.y + clampedRadiusTop;
                    rect.w = (float)config->width.right;
                    rect.h = (float)boundingBox.height - clampedRadiusTop - clampedRadiusBottom;
                    
                    SDL_RenderFillRectF(renderer, &rect);
                }

                if (config->width.top > 0) {
                    const float clampedRadiusLeft = SDL_min((float)config->cornerRadius.topLeft, maxRadius);
                    const float clampedRadiusRight = SDL_min((float)config->cornerRadius.topRight, maxRadius);
                    
                    SDL_FRect rect;
                    rect.x = boundingBox.x + clampedRadiusLeft;
                    rect.y = boundingBox.y;
                    rect.w = boundingBox.width - clampedRadiusLeft - clampedRadiusRight;
                    rect.h = (float)config->width.top;
                    
                    SDL_RenderFillRectF(renderer, &rect);
                }

                if (config->width.bottom > 0) {
                    const float clampedRadiusLeft = SDL_min((float)config->cornerRadius.bottomLeft, maxRadius);
                    const float clampedRadiusRight = SDL_min((float)config->cornerRadius.bottomRight, maxRadius);
                    
                    SDL_FRect rect;
                    rect.x = boundingBox.x + clampedRadiusLeft;
                    rect.y = boundingBox.y + boundingBox.height - config->width.bottom;
                    rect.w = boundingBox.width - clampedRadiusLeft - clampedRadiusRight;
                    rect.h = (float)config->width.bottom;
                    
                    SDL_RenderFillRectF(renderer, &rect);
                }

                //corner index: 0->3 topLeft -> CW -> bottonLeft
                if ((config->width.top > 0) && (config->cornerRadius.topLeft > 0)) {
                    SDL_RenderCornerBorder(renderer, &boundingBox, config, 0, config->color);
                }

                if ((config->width.top > 0) && (config->cornerRadius.topRight > 0)) {
                    SDL_RenderCornerBorder(renderer, &boundingBox, config, 1, config->color);
                }

                if ((config->width.bottom > 0) && (config->cornerRadius.bottomRight > 0)) {
                    SDL_RenderCornerBorder(renderer, &boundingBox, config, 2, config->color);
                }

                if ((config->width.bottom > 0) && (config->cornerRadius.bottomLeft > 0)) {
                    SDL_RenderCornerBorder(renderer, &boundingBox, config, 3, config->color);
                }
                break;
            }
            
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                // Custom render commands not implemented
                // Silently ignore instead of warning
                break;
            }
            
            default:
                // This catches truly invalid command types that aren't defined in the enum
                fprintf(stderr, "Error: Invalid render command type: %d\n", renderCommand->commandType);
                break;
        }
    }
}

#ifdef __cplusplus
}
#endif
