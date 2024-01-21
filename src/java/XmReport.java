import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.util.stream.Collectors.joining;

public class XmReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = " ";
  static String venue = " ";
  static String attend = " ";
  static List<String> evol = new ArrayList();
  static List<String> events = new ArrayList();
  static List<String> hRoster = new ArrayList();
  static List<String> aRoster = new ArrayList();
  static List<String> hSubs = new ArrayList();
  static List<String> aSubs = new ArrayList();
  static Set<String> hScorers = new HashSet();
  static Set<String> aScorers = new HashSet();
  static Map<String, Integer> min = new HashMap();
  static String hCoach = "      ";
  static String aCoach = "      ";
  static String referee = "      ";
  static Map<String, String> eSymbol = new HashMap();

  private static SimpleDateFormat sdf = new SimpleDateFormat("MMMMM dd, yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractReport(doc);
  }

  private static void extractReport(Document doc) {
    Elements headers = doc.select(".row");
    Elements sections = doc.select(".sb-ereignisse");
    Element einfo = doc.selectFirst(".sb-spieldaten");
    Element elineups = selectHeader(headers, "Line-Ups");
    Element egoals = selectSection(sections, "sb-tore");
    Element epens = selectSection(sections, "sb-verschossene");
    Element esubs = selectSection(sections, "sb-wechsel");
    Element ecards = selectSection(sections, "sb-karten");
    extractInfo(einfo);
    extractLineups(elineups);
    extractGoals(egoals);
    extractPenalties(epens);
    extractSubs(esubs);
    extractCards(ecards);
    print();
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + date);
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue);
    System.out.println("Attendance: " + attend);
    System.out.println("Evolution: " + evol);
    System.out.println("Events: " + events);
    System.out.println("Minutes: " + min);
    System.out.println("Home Roster: " + hRoster);
    System.out.println("Home Subs: " + hSubs);
    System.out.println("Home Coach: " + hCoach);
    System.out.println("Guest Roster: " + aRoster);
    System.out.println("Guest Subs: " + aSubs);
    System.out.println("Guest Coach: " + aCoach);
    System.out.println("Referee: " + referee);
    System.out.println("Lineups: " + serializeLineups());
    System.out.println("Events: " + serializeEvents());
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home).append(",")
        .append(away).append(",")
        .append(sc1).append(",")
        .append(String.format("%s@%s", date, time)).append(",A,")
        .append(round).append(",")
        .append(venue).append(",")
        .append(attend).append(", ,");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-hRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=hSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-aRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=aSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  private static Element selectSection(Elements sections, String id) {
    for (Element e : sections) {
      if (e.attr("id").equals(id)) return e;
    }
    return null;
  }

  private static Element selectHeader(Elements headers, String id) {
        for (Element e : headers) {
            Element h = e.selectFirst(".content-box-headline"); if (h==null) continue;
            Element h2 = h.selectFirst("h2"); if (h2==null) continue;
            String title = h2.childNode(0).toString().trim();
            if (id.equals(title)) return e;
        }
        return null;
  }

  private static void extractInfo(Element e) {
    if (e == null) return;
    Element info = e.parent();
    Elements eteams = info.select(".sb-team");
    Element data = e.selectFirst(".sb-spieldaten");
    Element ehome = eteams.get(0).selectFirst(".sb-vereinslink");
    Element eaway = eteams.get(1).selectFirst(".sb-vereinslink");
    home = getId(ehome);
    away = getId(eaway);
    Element edate = data.selectFirst(".sb-datum");
    Elements links = edate.select("a[href]");
    String sround = links.get(0).childNode(0).toString();
    String rparts[] = sround.split("\\.");
    round = rparts[0];
    String dlink = links.get(1).attr("href");
    String dparts[] = dlink.split("/");
    date = dparts[dparts.length-1];
    time = getTime(edate);
    Element escore = data.selectFirst(".sb-endstand");
    score = escore.childNode(0).toString().trim();
    Element eplace = data.selectFirst(".sb-zusatzinfos");
    Elements elinks = eplace.select("a[href]");
    if (elinks.size() > 0) {
      Element evenue = elinks.get(0);
      if (evenue.childNodeSize() > 0) {
        venue = elinks.get(0).childNode(0).toString();
      }
      Element eatt = eplace.selectFirst(":containsOwn(Attendance)");
      if (eatt != null) {
        String text = eatt.childNode(0).toString();
        String aparts[] = text.split(" ");
        if (aparts.length > 1) attend = aparts[1];
      }
    }
    if (elinks.size() > 1) {
      Element rlink = elinks.get(1);
      referee = getMnem(rlink.attr("href"));
    }
  }

  private static void extractLineups(Element e) {
    if (e==null) return;
    List<String> hl = Collections.emptyList();
    List<String> hs = Collections.emptyList();
    List<String> al = Collections.emptyList();
    List<String> as = Collections.emptyList();
    Element b = e.selectFirst(".box");
    Elements sections = b.select(".large-6");
    if (sections.size()!=2) {
      System.err.println("Error: number of lineups sections: " + sections.size());
    }
    Element eHome = sections.get(0);
    Element eAway = sections.get(1);
    Elements eph = eHome.select(".aufstellung-spieler-container");
    Elements epa = eAway.select(".aufstellung-spieler-container");
    if (eph.size() > 0) {
      hl = positionalLineups(eph);
      hs = positionalSubs(eHome, true);
    }
    if (hl.size() == 0) {
      hl = regularLineups(eHome, true);
    }
    if (epa.size() > 0) {
      al = positionalLineups(epa);
      as = positionalSubs(eAway, false);
    }
    if (al.size() == 0) {
      al = regularLineups(eAway, false);
    }
    hRoster.addAll(hl);
    hSubs.addAll(hs);
    aRoster.addAll(al);
    aSubs.addAll(as);
  }

  private static List<String> positionalLineups(Elements ep) {
    List<String> mnems = new ArrayList();
    if (ep.size() != 11) {
      System.err.println("Warning: lineup size " + ep.size());
    }
    for (int i=0; i<ep.size(); i++) {
      Element link = ep.get(i).selectFirst("a[href]");
      String mnem = link != null ? getMnem(link.attr("href")) : " ";
      if (mnem.length() > 1) mnems.add(mnem);
    }
    return mnems;
  }

  private static List<String> positionalSubs(Element e, boolean isHome) {
    List<String> mnems = new ArrayList();
    Elements esubs = e.select(".ersatzbank");
    if (esubs.size() > 0) {
      Element ehsubs = esubs.get(0);
      Elements rows = ehsubs.select("tr");
      for (Element es : rows) {
        Element eman = es.selectFirst(":containsOwn(Manager)");
        Element link = es.selectFirst("a[href]");
        String mnem = link != null ? getMnem(link.attr("href")) : " ";
        if (eman != null) {
          if (isHome) hCoach = mnem; else aCoach = mnem;
        } else {
          mnems.add(mnem);
        }
      }
    }
    return mnems;
  }

  private static List<String> regularLineups(Element e, boolean isHome) {
    List<String> mnems = new ArrayList();
    Elements tabs = e.select("tbody");
    Elements hr = tabs.size() > 0 ? tabs.get(0).select("tr") : new Elements();
    for (Element r : hr) {
      Elements hc = r.select("td");
      String pos = hc.size() > 0 ? hc.get(0).childNode(0).toString() : " ";
      Elements links = hc.size() > 1 ? hc.get(1).select("a[href]") : new Elements();
      if (pos.contains("Manager")) {
        String coach = links.size() > 0 ? getMnem(links.get(0).attr("href")) : " ";
        if (isHome) hCoach = coach; else aCoach = coach;
      } else {
        for (Element n : links) {
          mnems.add(getMnem(n.attr("href")));
        }
      }
    }
    return mnems;
  }

  private static void extractGoals(Element e) {
    if (e==null) return;
    Elements items = e.select("li");
    for (Element i : items) {
      Element epl = i.selectFirst(".sb-aktion");
      if (epl == null) continue;
      Element emin = epl.selectFirst(".sb-sprite-uhr-klein");
      int m = 90;
      if (emin!=null) {
        String style = emin.attr("style");
        m = decodeMinute(style);
      }
      Element eact = epl.selectFirst(".sb-aktion-aktion");
      Element elink = eact.selectFirst("a[href]");
      String px = getMnem(elink.attr("href"));
      String desc = eact.childNodeSize() > 2 ? eact.childNode(2).toString() : " ";
      String etype = "'";
      if (desc.contains("Penalty")) etype = "\"";
      if (desc.contains("Own")) etype = "`";
      events.add(String.format("%s%s%d", px, etype, m));
    }
  }

  private static void extractCards(Element e) {
    if (e==null) return;
    Elements items = e.select("li");
    for (Element i : items) {
      Element epl = i.selectFirst(".sb-aktion");
      if (epl == null) continue;
      Element emin = epl.selectFirst(".sb-sprite-uhr-klein");
      int m = 90;
      if (emin!=null) {
        String style = emin.attr("style");
        m = decodeMinute(style);
      }
      Element eact = epl.selectFirst(".sb-aktion-aktion");
      Element elink = eact.selectFirst("a[href]");
      String px = getMnem(elink.attr("href"));
      String desc = eact.childNodeSize() > 2 ? eact.childNode(2).toString() : " ";
      String etype = "#";
      if (desc.contains("Red") || desc.contains("Second")) {
        etype = "!";
        deductMin(px, m);
      }
      events.add(String.format("%s%s%d", px, etype, m));
    }
  }

  private static void extractPenalties(Element e) {
    if (e==null) return;
    Elements items = e.select("li");
    for (Element i : items) {
      String etype = i.className();
      Element epl = i.selectFirst(".sb-aktion");
      if (epl == null) continue;
      Element emin = epl.selectFirst(".sb-sprite-uhr-klein");
      int m = 90;
      if (emin!=null) {
        String style = emin.attr("style");
        m = decodeMinute(style);
      }
      Element eact = epl.selectFirst(".sb-aktion-aktion");
      Element eout = eact.selectFirst(".sb-aktion-wechsel-aus");
      Element elinko = eout.selectFirst("a[href]");
      String pxo = getMnem(elinko.attr("href"));
      events.add(String.format("%s|%d", pxo, m));
    }
  }

  private static void extractSubs(Element e) {
    if (e==null) return;
    Elements items = e.select("li");
    for (Element i : items) {
      String etype = i.className();
      Element epl = i.selectFirst(".sb-aktion");
      if (epl == null) continue;
      Element emin = epl.selectFirst(".sb-sprite-uhr-klein");
      int m = 90;
      if (emin!=null) {
        String style = emin.attr("style");
        m = decodeMinute(style);
      }
      Element eact = epl.selectFirst(".sb-aktion-aktion");
      Element eout = eact.selectFirst(".sb-aktion-wechsel-aus");
      Element ein = eact.selectFirst(".sb-aktion-wechsel-ein");
      Element elinko = eout.selectFirst("a[href]");
      Element elinki = ein.selectFirst("a[href]");
      String pxo = getMnem(elinko.attr("href"));
      String pxi = "-1";
      if (elinki!=null) {
        pxi = getMnem(elinki.attr("href"));
        if (etype.equals("sb-aktion-heim")) {
          if (!hSubs.contains(pxi)) hSubs.add(pxi);
        } else {
          if (!aSubs.contains(pxi)) aSubs.add(pxi);
        }
        min.put(pxi, 91 - m);
      }
      deductMin(pxo, m);
    }
  }

  private static int decodeMinute(String s) {
    String sparts[] = s.split(" ");
    String sx = sparts.length > 1 ?
        sparts[1].replaceAll("px", "")
            .replaceAll("-", "") : "0";
    String sy = sparts.length > 2 ?
        sparts[2].replaceAll("px", "")
            .replaceAll("-", "")
            .replaceAll(";", "") : "0";
    int x = Integer.valueOf(sx);
    int y = Integer.valueOf(sy);
    return 1 + 10*(y/36) + (x/36);
  }

  private static String extractLink(Node n, String def, int start, int end) {
    if (n==null) return def;
    String link = n.attr("href");
    List<String> parts = Arrays.asList(link.split("/"));
    int np = parts.size();
    if (start < 0) {
      return parts.subList(np + start, np + end).stream().collect(joining("/"));
    }
    else {
      return parts.subList(start, end).stream().collect(joining("/"));
    }
  }

  private static void deductMin(String name, int minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp - rem);
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static String getMnem(String s) {
    String parts[] = s.split("/");
    int i = 0; while (parts[i].length() == 0 && i < parts.length-3) i++;
    return parts[i] + "/" + parts[i+3];
  }

  private static String getTime(Element e) {
    String t = "00:00";
    if (e.childNodeSize() > 3) {
      Node n = e.childNode(e.childNodeSize()-1);
      String ns = n.toString().trim();
      String tns = ns.replaceAll("&nbsp;", "").replaceAll("\\|", "");
      String parts[] = tns.split(" ");
      String hparts[] = parts[0].split(":");
      int h = 0;
      try {
        h = Integer.valueOf(hparts[0]);
      } catch (NumberFormatException ex) { }
      int m = hparts.length>1 ? Integer.valueOf(hparts[1]) : 0;
      if (parts.length > 1 && parts[1].equals("PM")) h += 12;
      t = String.format("%02d:%02d", h, m);
    }
    return t;
  }

  private static String getId(Element e) {
    String id = e.attr("id");
    if (id == null || id.length() == 0) {
      Element ilink = e.selectFirst("a[href]");
      if (ilink != null) {
        String il = ilink.attr("href");
        String iparts[] = il.split("/");
        if (iparts.length > 4) id = iparts[4];
      }
    }
    return id;
  }

}
