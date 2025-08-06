#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clayman.hpp"

static bool claymaninstancehasbeencreated = false;

ClayMan::ClayMan(
    const uint32_t initialWidth, 
    const uint32_t initialHeight, 
    Clay_Dimensions (*measureTextFunction)(Clay_StringSlice text, Clay_TextElementConfig *config, void* userData),
    void* measureTextUserData
):windowWidth(initialWidth), windowHeight(initialHeight) {
    assert(claymaninstancehasbeencreated == false && "Only One Instance of ClayMan Should be Created!");
    if(windowWidth == 0){windowWidth = 1;}
    if(windowHeight == 0){windowHeight = 1;}
    
    // MEMORY OPTIMIZATION: Use minimal Clay arena instead of default huge allocation
    uint64_t clayRequiredMemory = 1024 * 1024 * 8; // 8MB instead of default ~64MB
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Dimensions dimensions = {};
    dimensions.width = (float)windowWidth;
    dimensions.height = (float)windowHeight;

    Clay_Initialize(clayMemory, dimensions, (Clay_ErrorHandler) handleErrors);

    Clay_SetMeasureTextFunction(measureTextFunction, measureTextUserData);

    claymaninstancehasbeencreated = true;
}

ClayMan::ClayMan(const uint32_t initialWidth, const uint32_t initialHeight):windowWidth(initialWidth), windowHeight(initialHeight){
    assert(claymaninstancehasbeencreated == false && "Only One Instance of ClayMan Should be Created!");
    if(windowWidth == 0){windowWidth = 1;}
    if(windowHeight == 0){windowHeight = 1;}
    claymaninstancehasbeencreated = true;
}

ClayMan::ClayMan(const ClayMan &clayMan){
    assert(false && "Do not pass clayMan by value, pass by reference!");
}

void ClayMan::updateClayState(
    const uint32_t width, 
    const uint32_t height, 
    const float mouseX, 
    const float mouseY, 
    const float scrollDeltaX, 
    const float scrollDeltaY, 
    const float frameTime, 
    const bool leftButtonDown
){
    windowWidth = width;
    windowHeight = height;
    if(windowWidth == 0){windowWidth = 1;}
    if(windowHeight == 0){windowHeight = 1;}
    
    Clay_Dimensions dimensions = {};
    dimensions.width = (float)windowWidth;
    dimensions.height = (float)windowHeight;
    Clay_SetLayoutDimensions(dimensions);

    Clay_Vector2 mousePos = {};
    mousePos.x = mouseX;
    mousePos.y = mouseY;
    Clay_SetPointerState(mousePos, leftButtonDown);

    Clay_Vector2 scrollDelta = {};
    scrollDelta.x = scrollDeltaX;
    scrollDelta.y = scrollDeltaY;
    Clay_UpdateScrollContainers(true, scrollDelta, frameTime);
}

void ClayMan::beginLayout(){
    start = std::chrono::high_resolution_clock::now();
    countFrames();
    resetStringArenaIndex();
    Clay_BeginLayout();
}

Clay_RenderCommandArray ClayMan::endLayout(){
    closeAllElements();
    measureTime();
    return Clay_EndLayout();
}

void ClayMan::element(){
    openElement();
    Clay_ElementDeclaration configs;
    applyElementConfigs(configs);
    closeElement();  
}

void ClayMan::element(Clay_ElementDeclaration configs, std::function<void()> childLambda) {
    
    openElement();
    applyElementConfigs(configs);
    if(childLambda != nullptr){
        childLambda();
    }
    closeElement();           
}

void ClayMan::element(Clay_ElementDeclaration configs){
    openElement();
    applyElementConfigs(configs);
    closeElement();
}

void ClayMan::element(std::function<void()> childLambda){
    openElement();
    Clay_ElementDeclaration configs;
    applyElementConfigs(configs);
    if(childLambda != nullptr){
        childLambda();
    }
    closeElement();
}

void ClayMan::openElement(Clay_ElementDeclaration configs){
    openElement();
    applyElementConfigs(configs);
}

void ClayMan::openElement(){
    Clay__OpenElement();
    openElementCount++;
}

void ClayMan::closeElement(){
    Clay__CloseElement();
    if(openElementCount <=0){
        if(!warnedAboutUnderflow){
            printf("Whoops! All elements are already closed!");
            warnedAboutUnderflow = true;
        }
    }else{
        openElementCount--;
    }
}

void ClayMan::textElement(const std::string& text, const Clay_TextElementConfig textElementConfig){
    Clay_String cs = toClayString(text);
    Clay__OpenTextElement(
        cs, 
        Clay__StoreTextElementConfig((Clay__Clay_TextElementConfigWrapper(textElementConfig)).wrapped)
    );
}

void ClayMan::textElement(const Clay_String& text, const Clay_TextElementConfig textElementConfig){
    Clay__OpenTextElement(
        text, 
        Clay__StoreTextElementConfig((Clay__Clay_TextElementConfigWrapper(textElementConfig)).wrapped)
    );
}

Clay_Sizing ClayMan::fixedSize(const uint32_t w, const uint32_t h) {
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis widthAxis = {};
    widthAxis.size.minMax.min = (float)w;
    widthAxis.size.minMax.max = (float)w;
    widthAxis.type = CLAY__SIZING_TYPE_FIXED;
    
    Clay_SizingAxis heightAxis = {};
    heightAxis.size.minMax.min = (float)h;
    heightAxis.size.minMax.max = (float)h;
    heightAxis.type = CLAY__SIZING_TYPE_FIXED;
    
    sizing.width = widthAxis;
    sizing.height = heightAxis;
    
    return sizing;
}

Clay_Sizing ClayMan::expandXY(){
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis widthAxis = {};
    widthAxis.type = CLAY__SIZING_TYPE_GROW;
    
    Clay_SizingAxis heightAxis = {};
    heightAxis.type = CLAY__SIZING_TYPE_GROW;
    
    sizing.width = widthAxis;
    sizing.height = heightAxis;
    
    return sizing;
}

Clay_Sizing ClayMan::expandX(){
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis widthAxis = {};
    widthAxis.type = CLAY__SIZING_TYPE_GROW;
    
    sizing.width = widthAxis;
    
    return sizing;
}

Clay_Sizing ClayMan::expandY(){
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis heightAxis = {};
    heightAxis.type = CLAY__SIZING_TYPE_GROW;
    
    sizing.height = heightAxis;
    
    return sizing;
}

Clay_Sizing ClayMan::expandXfixedY(const uint32_t h){
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis widthAxis = {};
    widthAxis.type = CLAY__SIZING_TYPE_GROW;
    
    Clay_SizingAxis heightAxis = {};
    heightAxis.size.minMax.min = (float)h;
    heightAxis.size.minMax.max = (float)h;
    heightAxis.type = CLAY__SIZING_TYPE_FIXED;
    
    sizing.width = widthAxis;
    sizing.height = heightAxis;
    
    return sizing;
}

Clay_Sizing ClayMan::expandYfixedX(const uint32_t w){
    Clay_Sizing sizing = {};
    
    Clay_SizingAxis widthAxis = {};
    widthAxis.size.minMax.min = (float)w;
    widthAxis.size.minMax.max = (float)w;
    widthAxis.type = CLAY__SIZING_TYPE_FIXED;
    
    Clay_SizingAxis heightAxis = {};
    heightAxis.type = CLAY__SIZING_TYPE_GROW;
    
    sizing.width = widthAxis;
    sizing.height = heightAxis;
    
    return sizing;
}

Clay_Padding ClayMan::padAll(const uint16_t p){
    Clay_Padding padding = {};
    padding.left = p;
    padding.right = p;
    padding.top = p;
    padding.bottom = p;
    return padding;
}

Clay_Padding ClayMan::padX(const uint16_t p){
    Clay_Padding padding = {};
    padding.left = p;
    padding.right = p;
    return padding;
}

Clay_Padding ClayMan::padY(const uint16_t p){
    Clay_Padding padding = {};
    padding.top = p;
    padding.bottom = p;
    return padding;
}

Clay_Padding ClayMan::padXY(const uint16_t px, const uint16_t py){
    Clay_Padding padding = {};
    padding.left = px;
    padding.right = px;
    padding.top = py;
    padding.bottom = py;
    return padding;
}

Clay_Padding ClayMan::padLeft(const uint16_t pl){
    Clay_Padding padding = {};
    padding.left = pl;
    return padding;
}

Clay_Padding ClayMan::padRight(const uint16_t pr){
    Clay_Padding padding = {};
    padding.right = pr;
    return padding;
}

Clay_Padding ClayMan::padTop(const uint16_t pt){
    Clay_Padding padding = {};
    padding.top = pt;
    return padding;
}

Clay_Padding ClayMan::padBottom(const uint16_t pb){
    Clay_Padding padding = {};
    padding.bottom = pb;
    return padding;
}

Clay_ChildAlignment ClayMan::centerXY(){
    Clay_ChildAlignment alignment = {};
    alignment.x = CLAY_ALIGN_X_CENTER;
    alignment.y = CLAY_ALIGN_Y_CENTER;
    return alignment;
}

Clay_ElementId ClayMan::hashID(const Clay_String& id){
    return Clay__HashString(id, 0, 0);
}

Clay_ElementId ClayMan::hashID(const std::string& id){
    return Clay__HashString(toClayString(id), 0, 0);
}

bool ClayMan::mousePressed(){
    return Clay_GetCurrentContext()->pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME;
}

bool ClayMan::pointerOver(const Clay_String id){
    return Clay_PointerOver(getClayElementId(id));
}

bool ClayMan::pointerOver(const std::string& id){
    return Clay_PointerOver(getClayElementId(toClayString(id)));
}

Clay_ElementId ClayMan::getClayElementId(const Clay_String id){
    return Clay_GetElementId(id);
}

Clay_ElementId ClayMan::getClayElementId(const std::string& id){
    return Clay_GetElementId(toClayString(id));
}

Clay_String ClayMan::toClayString(const std::string& str){
    int32_t length = (int32_t)str.size();
    const char* strchars = insertStringIntoArena(str);
    Clay_String cs = {};
    cs.length = (int32_t)str.size();
    cs.chars = strchars;
    return cs;
}

void ClayMan::applyElementConfigs(Clay_ElementDeclaration& configs){

    //This may need improved if clipping is used without scrolling
    if(configs.clip.horizontal || configs.clip.vertical){
        configs.clip.childOffset = Clay_GetScrollOffset();
    }

    Clay__ConfigureOpenElement((Clay__Clay_ElementDeclarationWrapper {configs}).wrapped);
}

void ClayMan::closeAllElements(){
    while(openElementCount > 0){
        if(!warnedAboutClose){
            printf("WARN: An element was not closed.");
            warnedAboutClose = true;
        }
        closeElement();
    }
}

int ClayMan::getWindowWidth(){
    return windowWidth;
}

int ClayMan::getWindowHeight(){
    return windowHeight;
}

uint32_t ClayMan::getFramecount(){
    return framecount;
}