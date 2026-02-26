#include <bits/stdc++.h> 
using namespace std;


class KMap{
public : 
    // To store minterms and don't care minterms 
    vector<int> minterms;
    vector<int> dontcares;
    vector<char> vars;

    vector<pair<int, int>> mins;
    set <pair<int, int>> covered;
    vector<string> booleanExpr;

    // constructor of the class 
    KMap (vector<char> vars, vector<int> minterms, vector<int> dontcares){
        this -> minterms = minterms ;
        this -> dontcares = dontcares;
        this -> vars = vars;
    }

    vector<vector<int>> grid = vector<vector<int>>(4, vector<int> (4, 0));

    // lookup array
    int gray_to_idx[4] = {0, 1, 3, 2};

    // to find {row, col} from a minterm
    pair<int, int> min_to_gray (int n){
        int row_bits = (n >> 2) & 3;
        int col_bits = n & 3;
        return {gray_to_idx[row_bits], gray_to_idx[col_bits]};
    } 

    int toBinary(pair<int, int> pr){
        int r_bits = gray_to_idx[pr.first];
        int c_bits = gray_to_idx[pr.second];
        return (r_bits << 2) | c_bits ;
    }
    // to create the vector mins containing both minterms and ones
    // void fillthemins(){
    //     for(int i=0; i< minterms.size(); i++){
    //         mins.push_back(minterms[i]);
    //     }
    //     for(int i=0; i< dontcares.size(); i++){
    //         mins.push_back(dontcares[i]);
    //     }
    // }



    // to fill the grid with 0, 1 or 2 corresp to type of minterm
    void fillthegrid(){

        for(int i=0; i<minterms.size(); i++){
            auto [row, col] = min_to_gray(minterms[i]);
            grid[row][col] = 1;
            mins.push_back({row, col});
        }

        for(int i=0; i<dontcares.size(); i++){
            auto [row, col] = min_to_gray(dontcares[i]);
            grid[row][col] = 2;
            mins.push_back({row, col});
        }
    }

    // To solve
    vector<vector<pair<int, int>>> solve (){
        vector<vector<pair<int, int>>> finalGroups;
        vector<vector<pair<int, int>>> Groups;
        vector<pair<int, int>> groupdim = {{4, 4}, {4, 2}, {2, 4},{2, 2}, {4, 1}, {1, 4}, {2, 1}, {1, 2}, {1, 1}};
        for(auto [nr, nc] : groupdim){
            for(auto [row, col] : mins){
                bool flag1 = true;
                bool flag2 = false;
                vector<pair<int, int>> group;
                for(int i=0; i<nr; i++){
                    for(int j = 0; j<nc; j++){
                        int ridx = (row + i) % 4;
                        int cidx = (col + j) % 4;

                        if ((grid[ridx][cidx] == 1) && (covered.find({ridx, cidx}) == covered.end())){
                            flag2 = true;
                        }

                        if (grid[ridx][cidx] == 0){
                            flag1 = false;
                            break;
                        }else{
                            group.push_back({ridx, cidx});
                        }
                    }
                    if(!flag1) break;
                }
                if(flag1 && flag2){
                    bool redundant = false;
                        for (auto& existing : Groups) {
                            if (existing.size() > group.size()) {
                                int match = 0;
                                for (auto p : group) for (auto ep : existing) if (p == ep) match++;
                                if (match == group.size()) { redundant = true; break; }
                            }
                        }
                        if (!redundant) Groups.push_back(group);
                }
            }
        }

        set<pair<int, int>> uncovered;
        for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) if (grid[r][c] == 1) uncovered.insert({r, c});

        while (!uncovered.empty()) {
            int bestPI = -1, maxCover = -1;
            for (int i = 0; i < Groups.size(); i++) {
                int count = 0;
                for (auto p : Groups[i]) if (uncovered.count(p)) count++;
                if (count > maxCover) { maxCover = count; bestPI = i; }
            }
            if (bestPI == -1) break;
            finalGroups.push_back(Groups[bestPI]);
            for (auto p : Groups[bestPI]) uncovered.erase(p);
        }
        return finalGroups;
    }
    vector<vector<pair<int, int>>> Groups;


    void BooleanExpr(){
        for(auto group: Groups){
            int mask = 0;
            int values = toBinary(group[0]);
            for(int i=1; i<group.size(); i++){
            mask |= (toBinary(group[i]) ^ values);
            }
            string term = "";
            for(int i=0; i< 4; i++){
                int bit = (1 << (3-i));
                if(!(mask & bit)) {
                   term += vars[i];
                   if(!(values & bit)) term += "'";
                  }
                }
                booleanExpr.push_back(term);
        }

        for(int i=0; i< booleanExpr.size(); i++){
            if(i == booleanExpr.size()-1){
                cout << booleanExpr[i] << endl;
            }else {
                cout << booleanExpr[i] <<  " + " ;
            }
        }
    }


    void generateSVG(string filename) {
        ofstream fout(filename);
        if (!fout) return;
    
        // --- Configuration ---
        const int S = 60;   // Square Size
        const int O = 100;  // Offset from top/left
        const int canvasSize = O + (4 * S) + 60;
        int idx_to_gray[4] = {0, 1, 3, 2}; // Reverse Gray Map
        vector<string> colors = {"rgba(255, 0, 0, 0.3)", "rgba(0, 0, 255, 0.3)", 
                                 "rgba(0, 128, 0, 0.3)", "rgba(255, 165, 0, 0.3)"};
    
        // 1. SVG Header
        fout << "<svg width='" << canvasSize << "' height='" << canvasSize + 80 
             << "' xmlns='http://www.w3.org/2000/svg' style='background:white;'>" << endl;
    
        // 2. Diagonal Header Logic
        fout << "<line x1='20' y1='20' x2='" << O << "' y2='" << O << "' stroke='black' stroke-width='2' />" << endl;
        fout << "<text x='" << O - 40 << "' y='40' font-family='Arial' font-size='18' font-style='italic'>" << vars[2] << vars[3] << "</text>" << endl;
        fout << "<text x='25' y='" << O - 10 << "' font-family='Arial' font-size='18' font-style='italic'>" << vars[0] << vars[1] << "</text>" << endl;
    
        // 3. Draw Grid, Axis Labels, and Values
        string labels[] = {"00", "01", "11", "10"};
        for (int i = 0; i < 4; i++) {
            fout << "<text x='" << O + i * S + (S / 2) << "' y='" << O - 10 << "' text-anchor='middle' font-family='Arial'>" << labels[i] << "</text>" << endl;
            fout << "<text x='" << O - 25 << "' y='" << O + i * S + (S / 2) + 5 << "' text-anchor='middle' font-family='Arial'>" << labels[i] << "</text>" << endl;
    
            for (int j = 0; j < 4; j++) {
                int x = O + j * S;
                int y = O + i * S;
                fout << "<rect x='" << x << "' y='" << y << "' width='" << S << "' height='" << S << "' fill='none' stroke='#ddd' />" << endl;
                
                string val = (grid[i][j] == 2) ? "X" : to_string(grid[i][j]);
                fout << "<text x='" << x + (S / 2) << "' y='" << y + (S / 2) + 6 << "' text-anchor='middle' font-family='Arial' font-size='20' font-weight='bold'>" << val << "</text>" << endl;
            }
        }
    
        
    
        // 4. Draw Group Loops (Wrap-around Logic)
        vector<string> finalTerms;
        int colorIdx = 0;
        for (const auto& group : Groups) {
            set<int> rows, cols;
            int fixed_mask = 0xF, values = -1;
    
            for (auto p : group) {
                rows.insert(p.first);
                cols.insert(p.second);
                // Bitmask logic for Boolean reduction
                int binValue = (idx_to_gray[p.first] << 2) | idx_to_gray[p.second];
                if (values == -1) values = binValue;
                else fixed_mask &= ~(binValue ^ values);
            }
    
            // Build Boolean String for this group
            string term = "";
            for(int b=0; b<4; b++) {
                int bit = 1 << (3-b);
                if (fixed_mask & bit) {
                    term += vars[b];
                    if (!(values & bit)) term += "'";
                }
            }
            if(term == "") term = "1"; 
            finalTerms.push_back(term);
    
            // Visual Loop logic
            string curCol = colors[colorIdx % colors.size()];
            bool wR = (rows.count(0) && rows.count(3) && !rows.count(1));
            bool wC = (cols.count(0) && cols.count(3) && !cols.count(1));
    
            auto draw = [&](float r, float c, float h, float w) {
                fout << "<rect x='" << O + c * S + 4 << "' y='" << O + r * S + 4 
                     << "' width='" << w * S - 8 << "' height='" << h * S - 8 
                     << "' rx='12' fill='" << curCol << "' stroke='" << curCol << "' stroke-width='2' />" << endl;
            };
    
            if (!wR && !wC) draw(*rows.begin(), *cols.begin(), rows.size(), cols.size());
            else if (wC && !wR) { draw(*rows.begin(), 0, rows.size(), 1); draw(*rows.begin(), 3, rows.size(), 1); }
            else if (wR && !wC) { draw(0, *cols.begin(), 1, cols.size()); draw(3, *cols.begin(), 1, cols.size()); }
            else { draw(0,0,1,1); draw(0,3,1,1); draw(3,0,1,1); draw(3,3,1,1); }
            colorIdx++;
        }
    
        // 5. Final Expression Legend
        string finalExpr = "";
        for(size_t i=0; i<finalTerms.size(); i++) {
            finalExpr += finalTerms[i] + (i == finalTerms.size()-1 ? "" : " + ");
        }
    
        
    
        int legY = O + (4 * S) + 60;
        fout << "<rect x='" << O << "' y='" << legY - 35 << "' width='" << (4 * S) << "' height='50' fill='#f4f4f4' rx='8' />" << endl;
        fout << "<text x='" << O + (2 * S) << "' y='" << legY << "' text-anchor='middle' font-family='Arial' font-size='20' font-weight='bold'>F = " << (finalExpr == "" ? "0" : finalExpr) << "</text>" << endl;
    
        fout << "</svg>";
        fout.close();
    }








    void pipeline(){
        fillthegrid();
        this -> Groups = solve();
        BooleanExpr();
        generateSVG("kmap.svg");
    }
};


int main(){

    int m1, m2;
    vector<int> minterms;
    vector<int> dontcares;
    vector<char> vars(4);
    cout << "Enter the names of the variables :";
    for(int i=0; i<4; i++){
        cin >> vars[i];
    } 

    
    cout << "Enter the number of Minterms :";
    cin >> m1;
    cout << "Enter the minterms :" << endl;
    for(int i=0; i<m1; i++){
        int temp;
        cin >> temp;
        if(temp > 15){
            cout << "Invalid minterm" << endl;
            return 1;
        }
        minterms.push_back(temp);
    }

    cout << "Enter the number of don't care minterms :";
    cin >> m2;
    cout << "Enter the Don't Care minterms :" << endl;
    for(int i=0; i< m2; i++){
        int temp;
        cin >> temp;
        if(temp > 15){
            cout << "Invalid Don't care minterms ;" << endl;
            return 1;
        }
        dontcares.push_back(temp);
    }

    KMap kmp1(vars, minterms, dontcares);
    kmp1.pipeline();

    cout << endl;
    for(int i=0; i<4; i++){
        for(int j = 0; j<4; j++){
            cout << kmp1.grid[i][j] << " ";
        }
        cout << endl;
    }

    for(auto grp : kmp1.Groups){
        cout << "Group :" << endl;
        for(auto [r, c] : grp){
            cout << "     " << r << " " << c << endl;
        }
        cout << endl;
    }
    return 0;
}