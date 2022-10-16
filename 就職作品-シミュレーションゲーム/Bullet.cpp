#include <memory>
#include "Bullet.h"
#include "dx11mathutil.h"
#include "CModel.h"
#include "CDirectInput.h"
#include "ModelMgr.h"

constexpr float BULLETSPEED = 5.0f;
constexpr uint32_t BULLETLIFE = 60;

bool Bullet::Init()
{
    bool sts = true;
    //行列初期化
    DX11MtxIdentity(m_mtx);

    return sts;
}

void Bullet::Update()
{
    //全弾更新
    for (auto& b : g_bullets) {
        b->m_life--;
        if (b->m_life > 0)
        {
            b->m_pos.x = b->m_pos.x + b->m_speed * b->m_direction.x;
            b->m_pos.y = b->m_pos.y + b->m_speed * b->m_direction.y;
            b->m_pos.z = b->m_pos.z + b->m_speed * b->m_direction.z;

            b->m_mtx._41 = b->m_pos.x;
            b->m_mtx._42 = b->m_pos.y;
            b->m_mtx._43 = b->m_pos.z;

        }
        else
        {
            b->m_sts = BULLETSTATUS::DEAD;
        }
    }
    //死んでいる弾削除
    auto it = g_bullets.begin();
    while (it != g_bullets.end())
    {
        if ((*it)->isLive() == false)
        {
            it = g_bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Bullet::Draw()
{
 //モデル描画
        //全弾描画
    for (auto& b : g_bullets) {
        if (b->m_sts == BULLETSTATUS::LIVE)
        {
            b->m_pmodel->Draw(b->m_mtx);
        }
    }

}

void Bullet::Finalize()
{
}

void Bullet::FireBullet(DirectX::XMFLOAT4X4 mtx, DirectX::XMFLOAT3 ppos)
{
    //std::unique_ptr<Bullet> p;

    //p = std::make_unique<Bullet>();

    ////弾の初期化
    //p->Init();

    ////モデルセット
    //p->SetModel(
    //    ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::MISSILE)].modelname)
    //);

    ////初期位置セット
    //p->SetPos(ppos);
    //p->SetDirection(mtx);
    ////最後尾に弾を追加
    //g_bullets.emplace_back(std::move(p));
}


