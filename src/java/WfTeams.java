import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Rewritten for the 2026 worldfootball.net redesign.
 *
 * Old version targeted transfermarkt.us's ".responsive-table" team list and
 * was never wired into any script in this repo (confirmed: no .sh anywhere
 * calls WfTeams). This version targets worldfootball.net's standings table
 * instead, replacing wf-get-teams.sh's old approach of grepping the
 * "/players/<league>-<season>/" listing page for team links (that page/
 * pattern no longer exists under the new scheme).
 *
 * Input: a season's results-and-standings page, e.g.
 *   https://www.worldfootball.net/competition/co123/portugal-primeira-liga/se96654/2025-2026/results-and-standings/
 * (any matchday works -- the standings table is the same on every one).
 *
 * Markup: div.hs-block.hs-standing > table > tr.standing, each with
 *   td.team-name a[href] -> https://www.worldfootball.net/teams/te<id>/<slug>/
 * There are two ".hs-standing" blocks on some pages (e.g. home/away split);
 * we read the first one, which is the overall table.
 *
 * Output: "id,slug,name" per team (one line per team), e.g.
 *   "te1776,sporting-cp,Sporting CP"
 * wf-get-teams.sh matches teams against data/webteams.dat by slug (as
 * before) and uses the id to build the new "/teams/te<id>/<slug>/" fetch
 * URL (the old URL had no id and needed none).
 */
public class WfTeams {

  public static void main(String args[]) {
    try { System.setOut(new java.io.PrintStream(System.out, true, "UTF-8")); } catch (Exception e) { /* ignore */ }
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractTeams(doc);
  }

  private static void extractTeams(Document doc) {
    Element standing = doc.selectFirst("div.hs-block.hs-standing");
    if (standing == null) {
      System.err.println("WfTeams: no 'hs-standing' block found.");
      return;
    }
    Element table = standing.selectFirst("table");
    if (table == null) {
      System.err.println("WfTeams: 'hs-standing' block has no table.");
      return;
    }
    for (Element row : table.select("tr.standing")) {
      Element link = row.selectFirst("td.team-name a[href]");
      if (link == null) continue;
      String[] idSlug = idAndSlug(link.attr("href"));
      if (idSlug == null) continue;
      String name = link.text().trim();
      System.out.println(String.format("%s,%s,%s", idSlug[0], idSlug[1], name));
    }
  }

  // "https://www.worldfootball.net/teams/te1776/sporting-cp/" -> {"te1776","sporting-cp"}
  private static String[] idAndSlug(String href) {
    List<String> segs = new ArrayList<>();
    for (String p : href.split("/")) if (p.length() > 0) segs.add(p);
    if (segs.size() < 2) return null;
    return new String[]{segs.get(segs.size() - 2), segs.get(segs.size() - 1)};
  }
}
