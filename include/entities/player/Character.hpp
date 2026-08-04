#pragma once
#include "Entity.hpp"
#include <functional>
#include <utility>

class Character: public Entity{
public:
    using CollisionResolver = std::function<void(Character&, sf::Time)>;

protected:
    int facing_ = 1;
    bool onGround_ = false;
    bool hitRoof_ = false;
    CollisionResolver collisionResolver_;
public:
    
    Character(sf:: Vector2f position, float width, float height) : Entity(position, width, height){}

    bool onGround() const{
        return onGround_;
    }

    void setOnGround(bool onGround){
        onGround_ = onGround;
    }

    void setHitRoof(bool hitRoof){
        hitRoof_ = hitRoof;
    }

    void setCollisionResolver(CollisionResolver resolver){
        collisionResolver_ = std::move(resolver);
    }

    void clearCollisionResolver(){
        collisionResolver_ = nullptr;
    }

    int facing() const{
        return facing_;
    }

    void setFacing(int facing){
        facing_ = facing < 0 ? -1 : 1;
    }

    void moveCharacter(sf::Time dt){
        if (collisionResolver_) {
            collisionResolver_(*this, dt);
            return;
        }

        setOnGround(false);
    
        sf::Vector2f position = position_;
        sf::Vector2f velocity = velocity_;
        
        const float width = Entity::width();
        const float height = Entity::height();

        // -------------------------------------------------------------
        // XỬ LÝ DI CHUYỂN & VA CHẠM THEO TRỤC X (NGANG)
        // -------------------------------------------------------------
        position.x += velocity.x * dt.asSeconds();

        // sf::FloatRect nhận tham số: (left, top, width, height)
        sf::FloatRect horizontal(position.x, position.y, width, height);
        
        // if (collidesSolid(horizontal)) {
        //     if (velocity.x > 0.0f) { // Đang sang phải -> va vào vách bên trái Tile
        //         const int tileX = static_cast<int>(std::floor((horizontal.left + horizontal.width) / kTileSize));
        //         position.x = static_cast<float>(tileX * kTileSize) - horizontal.width - 0.01f;
        //     } else if (velocity.x < 0.0f) { // Đang sang trái -> va vào vách bên phải Tile
        //         const int tileX = static_cast<int>(std::floor(horizontal.left / kTileSize));
        //         position.x = static_cast<float>((tileX + 1) * kTileSize) + 0.01f;
        //     }
        //     velocity.x = 0.0f;
        // }

        setPosition(position);

        // -------------------------------------------------------------
        // XỬ LÝ DI CHUYỂN & VA CHẠM THEO TRỤC Y (DỌC)
        // -------------------------------------------------------------
        position.y += velocity.y * dt.asSeconds();
        sf::FloatRect vertical(position.x, position.y, width, height);
///
        if (collidesSolid(vertical)) {
            if (velocity.y > 0.0f) { // Đang rơi xuống -> Chạm đất
                const int tileY = static_cast<int>(std::floor((vertical.top + vertical.height) / kTileSize));
                position.y = static_cast<float>(tileY * kTileSize) - vertical.height - 0.01f;
                character.setOnGround(true);
            } 
            else if (velocity.y < 0.0f) { // Đang nhảy lên -> Cụng đầu vào trần
                const int tileY = static_cast<int>(std::floor(vertical.top / kTileSize));
                
                if (Player* player = dynamic_cast<Player*>(&character)) {
                    const int left = static_cast<int>(std::floor(vertical.left / kTileSize));
                    const int right = static_cast<int>(std::floor((vertical.left + vertical.width - 1.0f) / kTileSize));
                    
                    for (int x = left; x <= right; ++x) {
                        handleBlockHit(*player, x, tileY);
                    }
                }
                position.y = static_cast<float>((tileY + 1) * kTileSize) + 0.01f;
                HitRoof_ = true;
            }
            velocity.y = 0.0f;
        }

        /* Test mới mặt đất ảo
        */
            // const float groundY = 500.0f;
            // if (velocity.y > 0.0f&& position.y + height >= groundY) {
            //     position.y = groundY - height; 
            //     velocity.y = 0.0f;
                
            //     setOnGround(true); 
            // }
        /**/

        // Cập nhật lại vị trí và vận tốc cuối cùng vào SFML Character
        setPosition(position);
        setVelocity(velocity);
    }

};
