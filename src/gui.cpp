#include "WaterSortSolver.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr unsigned W = 1280, H = 800;
const sf::Color BG(12, 18, 32), PANEL(22, 31, 50), PANEL2(29, 40, 64);
const sf::Color TEXT(235, 241, 250), MUTED(151, 165, 190), ACCENT(76, 201, 240);
const std::array<sf::Color, 12> PALETTE = {
    sf::Color(255, 86, 105), sf::Color(72, 149, 239), sf::Color(80, 200, 120),
    sf::Color(255, 190, 70), sf::Color(170, 100, 255), sf::Color(255, 120, 50),
    sf::Color(35, 205, 190), sf::Color(245, 100, 200), sf::Color(150, 210, 70),
    sf::Color(110, 120, 255), sf::Color(245, 225, 85), sf::Color(195, 120, 85)
};

sf::Color colorFor(int c) { return c <= 0 ? sf::Color::Transparent : PALETTE[(c - 1) % PALETTE.size()]; }

bool loadFont(sf::Font& font) {
    const std::vector<std::string> paths = {
        "assets/Inter-Regular.ttf", "assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "C:/Windows/Fonts/segoeui.ttf", "/System/Library/Fonts/Supplemental/Arial.ttf"
    };
    for (const auto& p : paths) if (std::filesystem::exists(p) && font.loadFromFile(p)) return true;
    return false;
}

void roundedRect(sf::RenderTarget& target, sf::FloatRect r, float radius, sf::Color fill, sf::Color outline = sf::Color::Transparent, float thickness = 0) {
    sf::RectangleShape center({r.width - 2 * radius, r.height}); center.setPosition(r.left + radius, r.top); center.setFillColor(fill); target.draw(center);
    sf::RectangleShape middle({r.width, r.height - 2 * radius}); middle.setPosition(r.left, r.top + radius); middle.setFillColor(fill); target.draw(middle);
    for (int ix = 0; ix < 2; ++ix) for (int iy = 0; iy < 2; ++iy) {
        sf::CircleShape c(radius, 24); c.setFillColor(fill); c.setPosition(r.left + ix * (r.width - 2 * radius), r.top + iy * (r.height - 2 * radius)); target.draw(c);
    }
    if (thickness > 0) {
        sf::RectangleShape border({r.width, r.height}); border.setPosition(r.left, r.top); border.setFillColor(sf::Color::Transparent); border.setOutlineColor(outline); border.setOutlineThickness(thickness); target.draw(border);
    }
}

void label(sf::RenderTarget& t, const sf::Font& f, const std::string& s, unsigned size, sf::Vector2f pos, sf::Color c = TEXT, bool bold = false) {
    sf::Text x(s, f, size); x.setPosition(pos); x.setFillColor(c); if (bold) x.setStyle(sf::Text::Bold); t.draw(x);
}

struct Button { sf::FloatRect box; std::string text; bool primary{}; bool enabled{true}; };
void drawButton(sf::RenderTarget& t, const sf::Font& f, const Button& b, sf::Vector2f mouse) {
    bool hover = b.enabled && b.box.contains(mouse);
    sf::Color fill = b.primary ? (hover ? sf::Color(92, 214, 248) : ACCENT) : (hover ? sf::Color(48, 62, 88) : PANEL2);
    if (!b.enabled) fill = sf::Color(42, 48, 62);
    roundedRect(t, b.box, 10, fill);
    sf::Text tx(b.text, f, 16); tx.setStyle(sf::Text::Bold); tx.setFillColor(b.primary ? BG : (b.enabled ? TEXT : MUTED));
    auto q = tx.getLocalBounds(); tx.setPosition(b.box.left + (b.box.width-q.width)/2-q.left, b.box.top+(b.box.height-q.height)/2-q.top-1); t.draw(tx);
}

void drawTube(sf::RenderTarget& target, const sf::Font& font, const Tube& tube, int capacity, float x, float y, float scale, int number, bool selected, bool source, float lift=0) {
    const float tw = 72 * scale, th = 245 * scale, wall = 5 * scale;
    y -= lift;
    if (selected || source) {
        sf::CircleShape halo(tw * .67f, 48); halo.setOrigin(halo.getRadius(), halo.getRadius()); halo.setPosition(x + tw/2, y + th/2); halo.setFillColor(source ? sf::Color(255,190,70,30) : sf::Color(76,201,240,32)); target.draw(halo);
    }
    sf::RectangleShape glass({tw, th}); glass.setPosition(x,y); glass.setFillColor(sf::Color(225,240,255,18)); glass.setOutlineThickness(wall); glass.setOutlineColor(selected ? ACCENT : sf::Color(195,215,235,150)); target.draw(glass);
    sf::RectangleShape rim({tw + 12*scale, 10*scale}); rim.setPosition(x-6*scale,y-5*scale); rim.setFillColor(sf::Color(215,230,245,210)); target.draw(rim);
    const float innerW = tw - 2*wall, innerH = th - 2*wall;
    const float layerH = innerH / capacity;
    for (int i=0;i<static_cast<int>(tube.size());++i) {
        sf::RectangleShape liquid({innerW, layerH-1}); liquid.setPosition(x+wall, y+th-wall-(i+1)*layerH); liquid.setFillColor(colorFor(tube[i])); target.draw(liquid);
        sf::RectangleShape shine({innerW*.16f, layerH-4}); shine.setPosition(x+wall+4, y+th-wall-(i+1)*layerH+2); shine.setFillColor(sf::Color(255,255,255,45)); target.draw(shine);
    }
    label(target,font,"T"+std::to_string(number),15,{x+tw/2-10*scale,y+th+13*scale},selected?ACCENT:MUTED,true);
}

State preset(int id, int& cap) {
    if (id==1) { cap=2; return {{1,2},{2,1},{},{}}; }
    if (id==2) { cap=4; return {{1,2,3,4},{2,4,1,3},{3,1,4,2},{4,3,2,1},{},{}}; }
    cap=4; return {{1,2,1,3},{2,3,4,1},{3,4,2,4},{4,1,3,2},{},{}};
}
}

int main() {
    sf::RenderWindow window(sf::VideoMode(W,H), "Chromaflow - Colored Water Sort Solver", sf::Style::Titlebar|sf::Style::Close);
    window.setFramerateLimit(60);
    sf::Font font; if (!loadFont(font)) return 2;

    int capacity=4; State initial=preset(2,capacity), shown=initial;
    SolveResult solution; int step=0; bool playing=false; sf::Clock autoplay; int selected=-1; std::string status="Choose a preset or edit tubes, then solve.";
    int presetId=2;

    auto rebuildToStep=[&]{ shown=initial; for(int i=0;i<step && i<(int)solution.moves.size();++i) WaterSortSolver::makePour(shown,capacity,solution.moves[i].from,solution.moves[i].to); };
    auto solve=[&]{ try { WaterSortSolver s(capacity,initial); solution=s.solveMinimumMoves(); step=0; shown=initial; playing=false; status=solution.solved ? "Optimal solution ready." : "No solution exists for this arrangement."; } catch(const std::exception& e){status=e.what();} };

    while(window.isOpen()) {
        sf::Vector2f mouse=window.mapPixelToCoords(sf::Mouse::getPosition(window));
        Button solveBtn{{1040,88,190,46},"SOLVE OPTIMALLY",true,true};
        Button prevBtn{{920,708,84,42},"< PREV",false,solution.solved&&step>0};
        Button playBtn{{1015,708,92,42},playing?"PAUSE":"PLAY",true,solution.solved&&!solution.moves.empty()};
        Button nextBtn{{1118,708,84,42},"NEXT >",false,solution.solved&&step<(int)solution.moves.size()};
        Button resetBtn{{1040,145,190,40},"RESET BOARD",false,true};

        sf::Event e{};
        while(window.pollEvent(e)) {
            if(e.type==sf::Event::Closed) window.close();
            if(e.type==sf::Event::KeyPressed) {
                if(e.key.code==sf::Keyboard::Space && solution.solved) playing=!playing;
                if(e.key.code==sf::Keyboard::Right && step<(int)solution.moves.size()){++step;rebuildToStep();}
                if(e.key.code==sf::Keyboard::Left && step>0){--step;rebuildToStep();}
                if(e.key.code==sf::Keyboard::S) solve();
                if(e.key.code==sf::Keyboard::R){shown=initial;step=0;playing=false;}
            }
            if(e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                if(solveBtn.box.contains(mouse)) solve();
                else if(resetBtn.box.contains(mouse)){initial=preset(presetId,capacity);shown=initial;solution={};step=0;playing=false;selected=-1;status="Board reset.";}
                else if(prevBtn.enabled&&prevBtn.box.contains(mouse)){--step;rebuildToStep();playing=false;}
                else if(nextBtn.enabled&&nextBtn.box.contains(mouse)){++step;rebuildToStep();playing=false;}
                else if(playBtn.enabled&&playBtn.box.contains(mouse)) playing=!playing;
                else {
                    for(int p=1;p<=3;++p) {
                        sf::FloatRect box(920+(p-1)*98,215,88,38);
                        if(box.contains(mouse)){presetId=p;initial=preset(p,capacity);shown=initial;solution={};step=0;playing=false;selected=-1;status="Preset loaded.";}
                    }
                    const int n=static_cast<int>(shown.size()); float available=820.f; float scale=std::min(1.f, available/(n*105.f)); float total=n*105.f*scale; float start=55+(820-total)/2;
                    for(int i=0;i<n;++i){sf::FloatRect hit(start+i*105*scale,300,72*scale,275*scale); if(hit.contains(mouse)){
                        if(selected<0) selected=i; else { if(WaterSortSolver::canPour(initial,capacity,selected,i)){WaterSortSolver::makePour(initial,capacity,selected,i);shown=initial;solution={};step=0;status="Manual move applied.";} selected=-1;} break;}}
                }
            }
        }
        if(playing && autoplay.getElapsedTime().asSeconds()>.65f) { autoplay.restart(); if(step<(int)solution.moves.size()){++step;rebuildToStep();} else playing=false; }

        window.clear(BG);
        // Header
        label(window,font,"CHROMAFLOW",30,{48,28},TEXT,true); label(window,font,"MINIMUM-MOVE WATER SORT SOLVER",13,{50,66},ACCENT,true);
        sf::RectangleShape line({1184,1}); line.setPosition(48,84); line.setFillColor(sf::Color(80,100,130,80)); window.draw(line);
        // Main board panel
        roundedRect(window,{38,105,850,640},18,PANEL);
        label(window,font,"Puzzle Board",24,{62,130},TEXT,true);
        label(window,font,"Click a source tube, then a destination tube to edit manually.",14,{62,164},MUTED);

        int source=-1,dest=-1; if(solution.solved && step<(int)solution.moves.size()){source=solution.moves[step].from;dest=solution.moves[step].to;}
        const int n=static_cast<int>(shown.size()); float available=800.f; float scale=std::min(1.f,available/(n*105.f)); float total=n*105.f*scale; float start=63+(800-total)/2;
        for(int i=0;i<n;++i) drawTube(window,font,shown[i],capacity,start+i*105*scale,300,scale,i+1,i==selected,i==source,(i==source&&playing)?6.f:0.f);

        roundedRect(window,{62,640,800,76},12,PANEL2);
        std::string moveText;
        if(solution.solved && solution.moves.empty()) moveText="The puzzle is already solved.";
        else if(solution.solved && step<(int)solution.moves.size()) moveText="Next: pour Tube "+std::to_string(source+1)+" into Tube "+std::to_string(dest+1)+" ("+std::to_string(solution.moves[step].amount)+" layer"+(solution.moves[step].amount==1?"":"s")+")";
        else if(solution.solved) moveText="Solution complete - every non-empty tube contains one color.";
        else moveText=status;
        label(window,font,moveText,17,{82,657},TEXT,true);
        label(window,font,"Step "+std::to_string(step)+" / "+std::to_string(solution.moves.size()),14,{82,686},MUTED);

        // Sidebar
        roundedRect(window,{908,105,334,640},18,PANEL);
        drawButton(window,font,solveBtn,mouse); drawButton(window,font,resetBtn,mouse);
        label(window,font,"PRESETS",13,{926,205},MUTED,true);
        for(int p=1;p<=3;++p){Button b{{920.f+(p-1)*98,215,88,38},p==1?"Classic":p==2?"Medium":"Mixed",false,true};drawButton(window,font,b,mouse); if(p==presetId){sf::RectangleShape u({62,2});u.setPosition(b.box.left+13,b.box.top+35);u.setFillColor(ACCENT);window.draw(u);}}

        label(window,font,"SOLUTION",13,{926,282},MUTED,true);
        roundedRect(window,{920,304,310,118},12,PANEL2);
        label(window,font,solution.solved?std::to_string(solution.moves.size()):"--",38,{942,322},solution.solved?ACCENT:TEXT,true);
        label(window,font,"minimum moves",14,{942,370},MUTED);
        label(window,font,solution.solved?std::to_string(solution.visitedStates):"--",24,{1090,329},TEXT,true);
        label(window,font,"states explored",12,{1090,370},MUTED);

        label(window,font,"ALGORITHM",13,{926,450},MUTED,true);
        roundedRect(window,{920,473,310,158},12,PANEL2);
        label(window,font,"Breadth-First Search",19,{940,491},TEXT,true);
        label(window,font,"Guarantees the shortest valid",14,{940,526},MUTED);
        label(window,font,"sequence of pours by exploring",14,{940,549},MUTED);
        label(window,font,"states level by level.",14,{940,572},MUTED);
        label(window,font,"SPACE: play/pause   S: solve",12,{940,604},ACCENT);
        drawButton(window,font,prevBtn,mouse);drawButton(window,font,playBtn,mouse);drawButton(window,font,nextBtn,mouse);

        window.display();
    }
}
