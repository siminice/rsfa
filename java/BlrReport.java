import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.nodes.TextNode;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.util.stream.Collectors.joining;

public class BlrReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = "00:00";
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
  static String curp = " ";

  private static SimpleDateFormat sdf = new SimpleDateFormat("dd.MM.yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  static {
    eSymbol.put("goal", "'");
    eSymbol.put("goal11", "\"");
    eSymbol.put("goala", "`");
    eSymbol.put("iconff-card yellow", "#");
    eSymbol.put("iconff-dcard yellow", "#");
    eSymbol.put("iconff-card red", "!");
    eSymbol.put("iconff-dcard red", "!");
    eSymbol.put("goal11n", "/");
  };

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://footballfacts.ru");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".elgg-main");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Elements sections = content.select(".elgg-row");
    extractRound(sections.get(0));
    extractTeams(sections.get(1));
    extractLocation(sections.get(2));
    extractRef(sections.get(3));
    Element lineups = findSection(sections, "Составы");
    extractLineups(lineups);
    print();
  }

  private static Element findSection(Elements sections, String label) {
    String query = String.format(":contains(%s)", label);
    for (Element s : sections) {
      Element x = s.selectFirst(query);
      if (x!=null) return s;
    }
    return null;
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + toIso(date));
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
        .append(String.format("%s@%s", toIso(date), time)).append(",A,")
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

  private static void extractLineups(Element el) {
    Elements divs = el.children();
    Elements erows = divs.get(0).select(".elgg-row");
    Integer subm = 91;
    int nr = erows.size();
    for (int i=0; i<11; i++) {
      Element row = erows.get(i);
      Elements cols = row.children();
      if (cols.size() > 0) {
        Element hcol = cols.get(0);
        Element ehpi = hcol.selectFirst(".bigfont");
        Element plink = ehpi.selectFirst("a[href]");
        String hpi = extractLink(plink, " ", -1, 0);
        hRoster.add(hpi);
        for (Element sub : hcol.children()) {
          if (sub.tagName().equals("div")) {
            if (sub.className().equals("bigfont")) addEvents(hpi, sub, 0);
            else { curp = addSubs(hpi, sub, 0); hpi = curp; }
          }
        }
      }
      if (cols.size() > 2) {
        Element acol = cols.get(2);
        Element eapi = acol.selectFirst(".bigfont");
        Element plink = eapi.selectFirst("a[href]");
        String api = extractLink(plink, " ", -1, 0);
        aRoster.add(api);
        for (Element sub : acol.children()) {
          if (sub.tagName().equals("div")) {
            if (sub.className().equals("bigfont")) addEvents(api, sub, 1);
            else { curp = addSubs(api, sub, 1); api = curp; }
          }
        }
      }
    }
    if (nr >= 13) {
      Elements esubs = erows.get(11).children();
      if (esubs.size() > 2) {
        Elements hslinks = esubs.get(0).select("a[href]");
        Elements aslinks = esubs.get(2).select("a[href]");
        for (Element hslink : hslinks) {
          String hsub = extractLink(hslink, " ", -1, 0);
          hSubs.add(hsub);
        }
        for (Element aslink : aslinks) {
          String asub = extractLink(aslink, " ", -1, 0);
          aSubs.add(asub);
        }
      }
    }
    if (nr >= 12) {
      Element ecos = erows.get(nr-1);
      Elements ecc = ecos.children();
      hCoach = extractLink(ecc.get(0), " ", -1, 0);
      aCoach = extractLink(ecc.get(2), " ", -1, 0);
    }
  }

  private static void extractRound(Element header) {
    if (header == null) return;
    Element ecol = header.selectFirst(".elgg-col");
    if (ecol == null) return;
    int nc = ecol.childNodeSize();
    String sround = ecol.childNode(nc-1).toString();
    round = sround.split("\\-")[0];
  }

  private static void extractTeams(Element e) {
    if (e==null) return;
    Elements links = e.select("a[href]");
    Elements cols = e.select("elgg-col");
    home = extractLink(links.get(0), " ", -1, 0);
    away = extractLink(links.get(2), " ", -1, 0);
    Element escore = e.selectFirst(".schet");
    score = escore.childNode(0).toString().replaceAll("&nbsp;", "");
  }

  private static void extractLocation(Element e) {
    if (e==null) return;
    Element ecol = e.selectFirst("[class*=elgg-col]");
    int nc = ecol.childNodeSize();
    Node ndate = ecol.childNode(0);
    String sdate = ndate.toString().replaceAll("\n", "").trim();
    String sparts[] = sdate.split(" ");
    date = sparts[0];
    time = sparts.length > 1 ? sparts[1] : "00:00";
    Elements slinks = ecol.select("a[href]");
    int ns = slinks.size();
    venue = extractLink(slinks.get(ns-1), " ", -1, 0);
    Node natt = ecol.childNode(nc-1);
    String satt = natt.toString().replaceAll("\n", "").trim();
    String aparts[] = satt.split(" ");
    attend = aparts.length > 1 ? aparts[1].replaceAll("&nbsp;", "") : " ";
  }

  private static void extractRef(Element e) {
    if (e==null) return;
    Element rlink = e.selectFirst("a[href]");
    referee = extractLink(rlink, " ", -1, 0);
  }

  private static String extractLink(Element e, String def, int start, int end) {
    if (e == null) return def;
    Node n = e.selectFirst("a[href]");
    return extractLink(n, def, start, end);
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

  private static void addMin(String name, Integer minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp + rem);
  }

  private static void addEvents(String pl, Element e, int ha) {
    for (Element dec : e.children()) {
      if (dec.tagName().equals("span")) addEvent(pl, dec, ha);
    }
  }

  private static void addEvent(String pl, Element dec, int ha) {
    String dt = dec.className();
    Integer m = extractMin(dec);
//    System.out.println("Evt: " + dt);
    if (dt.contains("event-goal")) {
      events.add(pl+'\''+m);
    }
    else if (dt.contains("event-agoal")) {
      events.add(pl+'`'+m);
    }
    else if (dt.contains("event-penal")) {
      events.add(pl+'"'+m);
    }
    else if (dt.contains("event-npenal")) {
      events.add(pl+'/'+m);
    }
    else if (dt.contains("event-warn")) {
      events.add(pl+'#'+m);
    }
    else if (dt.contains("event-ban")) {
      events.add(pl+'!'+m);
    }
    else if (dt.contains("event-dwarn")) {
      events.add(pl+'!'+m);
    }
  }

  private static String addSubs(String pl, Element sub, int ha) {
    String sn = extractLink(sub, " ", -1, 0);
    if (sn.equals(pl)) return pl;
    Integer ms = extractSubmin(sub);
    addMin(sn, ms);
    deductMin(pl, ms);
    if (ha==0) hSubs.add(sn); else aSubs.add(sn);
    for (Element ssub : sub.children()) {
      if (ssub.tagName().equals("div")) addSubs(pl, ssub, ha);
      if (ssub.tagName().equals("span")) addEvent(sn, ssub, ha);
    }
    return sn;
  }

  private static Integer extractSubmin(Element e) {
    for (Node n : e.childNodes()) {
      if (n instanceof TextNode) {
        String v = n.toString().trim();
        if (v.length() > 0) return Integer.valueOf(v);
      }
    }
    return Integer.valueOf(91);
  }

  private static void deductMin(String name, Integer minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp - rem);
  }

  private static Integer extractMin(Element emin) {
    if (emin == null) return 0;
    String s = emin.attr("title");
    String parts[] = s.split(" ");
    int np = parts.length;
    String clean = parts[np-1];
    Integer m = extra(clean, 1);
    return m;
  }

  private static Integer extra(String s, int x) {
    String[] parts = s.split("\\+");
    if (parts.length > 1) {
      int main = Integer.parseInt(parts[0]);
      int ex = Integer.parseInt(parts[1]);
      if (main == 45) return 45;
      return x > 0 ? main+ex : main;
    }
    Integer m = 0;
    try {
      m = Integer.parseInt(parts[0]);
    } catch (NumberFormatException e) {
      // zero
    }
    return m;
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static String trim(String s) {
    return s
        .replaceAll("\n", "")
        .replaceAll("^\\s+","").replaceAll("\\s+$","");
  }
}
