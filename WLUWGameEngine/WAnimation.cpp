#include "WAnimation.h"

WAnimation::WAnimation(WTexture* tex, Vector2 imageCount, float switchTime)
{
    this->imageCount = imageCount;
    this->switchTime = switchTime;
    totalTime = 0.0f;
    currentImage.x = 0;

    uvRect.w = tex->getSize().x / static_cast<float>(imageCount.x);
    uvRect.h = tex->getSize().y / static_cast<float>(imageCount.y);
}

void WAnimation::update(int row, int colBegin, int colEnd, float deltaTime, bool faceRight)
{
    currentImage.y = row;
    totalTime += deltaTime;

    if (switchTime != 0.0f)
    {
        if (totalTime >= switchTime)
        {
            totalTime -= switchTime;
            currentImage.x++;
            if (currentImage.x < colBegin)
            {
                currentImage.x = colBegin;
            }
            else if (currentImage.x >= imageCount.x || currentImage.x > colEnd)
            {
                currentImage.x = colBegin;
            }
        }
    }
    else
    {
        currentImage.x = colBegin;
    }

    uvRect.y = currentImage.y * uvRect.h;

    if (faceRight)
    {
        uvRect.x = currentImage.x * uvRect.w;
        uvRect.w = abs(uvRect.w);
    }
    else
    {
        uvRect.x = (currentImage.x + 1) * abs(uvRect.w);
        uvRect.w = -abs(uvRect.w);
    }

    ///Provide information
    this->row = row;
    this->col = currentImage.x;
    this->colBegin = colBegin;
    this->colEnd = colEnd;
}

void WAnimation::setTexture(WTexture* tex)
{
    uvRect.w = tex->getSize().x / static_cast<float>(imageCount.x);
    uvRect.h = tex->getSize().y / static_cast<float>(imageCount.y);
}

void WAnimation::setSwitchTime(float switchTime)
{
    this->switchTime = switchTime;
}